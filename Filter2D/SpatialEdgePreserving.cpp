#include "SpatialEdgePreserving.h"

bool DomainTransformFilter(cv::Mat _src, cv::Mat& _dst, int _sigma_s, double _sigma_r, int _num_iterations, DOMAIN_TRANSFORM_MODE _mode)
{
	switch (_mode)
	{
	case VERSION_1:
		return SpatialEdgePreservingFilter_v1(_src, _dst, _sigma_s, _sigma_r, _num_iterations);
		break;

	case NORMALIZED_CONVOLUTION:
		return SpatialEdgePreservingFilter_v2(_src, _dst, _sigma_s, _sigma_r, _num_iterations, NORMALIZED_CONVOLUTION);
		break;

	case INTERPOLATED_CONVOLUTION:
		return SpatialEdgePreservingFilter_v2(_src, _dst, _sigma_s, _sigma_r, _num_iterations, INTERPOLATED_CONVOLUTION);
		break;

	case RECURSIVE_FILTER:
		return SpatialEdgePreservingFilter_v2(_src, _dst, _sigma_s, _sigma_r, _num_iterations, RECURSIVE_FILTER);
		break;
	}

	return false;
}

int largerMin(cv::Mat src, float value, int offset)
{
	// 1차원 행렬만 가능
	for (int i = offset; i < src.cols; i++)
	{
		if (src.at<float>(i) > value)
		{
			return i - offset;
		}
	}

	return 0;
}

bool TransformedDomainBoxFilter_Horizontal(cv::Mat src, cv::Mat& dst, cv::Mat ct, double box_radius)
{
	//TIMER();

	std::chrono::steady_clock::time_point start, end;
	std::chrono::duration<double, std::milli> elapsed;

	/***********************************************************************************************
	* Normalized Convolution
	* Interpolated Convolution, Recursive Filter는 추후 구현 필요
	***********************************************************************************************/
	cv::Mat temp = cv::Mat::zeros(src.rows, src.cols, CV_32F);
	cv::Mat SAT = cv::Mat::zeros(src.rows, src.cols + 1, CV_32F);

	#pragma omp parallel for collapse(2)
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			SAT.at<float>(i, j + 1) = src.at<float>(i, j);
		}
	}

	cv::Mat l_pos = ct - box_radius;
	cv::Mat u_pos = ct + box_radius;

	cv::Mat l_idx = cv::Mat::zeros(src.rows, src.cols, CV_32S);
	cv::Mat u_idx = cv::Mat::zeros(src.rows, src.cols, CV_32S);

	cv::Mat inf_mat = cv::Mat_<float>(1, 1) << INFINITY;
	
	#pragma omp parallel for
	for (int row = 0; row < src.rows; row++)
	{
		cv::Mat xform_domain_pos_row = ct.row(row);
		cv::hconcat(xform_domain_pos_row, inf_mat, xform_domain_pos_row);

		cv::Mat l_pos_row = l_pos.row(row);
		cv::Mat u_pos_row = u_pos.row(row);

		cv::Mat local_l_idx = cv::Mat::zeros(1, src.cols, CV_32S);
		cv::Mat local_u_idx = cv::Mat::zeros(1, src.cols, CV_32S);

		local_l_idx.at<int>(0) = largerMin(xform_domain_pos_row, l_pos_row.at<float>(0));
		local_u_idx.at<int>(0) = largerMin(xform_domain_pos_row, u_pos_row.at<float>(0));

		for (int col = 1; col < src.cols; col++)
		{
			local_l_idx.at<int>(col) = local_l_idx.at<int>(col - 1) + largerMin(xform_domain_pos_row, l_pos_row.at<float>(col), local_l_idx.at<int>(col - 1));
			local_u_idx.at<int>(col) = local_u_idx.at<int>(col - 1) + largerMin(xform_domain_pos_row, u_pos_row.at<float>(col), local_u_idx.at<int>(col - 1));
		}

		local_l_idx.copyTo(l_idx.row(row));
		local_u_idx.copyTo(u_idx.row(row));
	}

	#pragma omp parallel for
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 1; j <= src.cols; j++)
		{
			SAT.at<float>(i, j) = SAT.at<float>(i, j) + SAT.at<float>(i, j - 1);
		}
	}
	
	#pragma omp parallel for collapse(2)
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			temp.at<float>(i, j) = (SAT.at<float>(i, u_idx.at<int>(i, j)) - SAT.at<float>(i, l_idx.at<int>(i, j))) / (u_idx.at<int>(i, j) - l_idx.at<int>(i, j));
		}
	}

	temp.copyTo(dst);
	return true;
}

bool SpatialEdgePreservingFilter_v1(cv::Mat _src, cv::Mat & _dst, int _sigma_s, double _sigma_r, int _num_iterations)
{
	TIMER();
	//TIMER_CUDA();

	std::chrono::steady_clock::time_point start, end;
	std::chrono::duration<double, std::milli> elapsed;

	if (_src.empty() || _src.channels() > 1)
	{
		std::cout << "Invalid Input Image. (Image is empty or has multiple channels.)" << std::endl;
		return false;
	}

	/******************
	* Domain Transform
	******************/
	cv::Mat src_temp;
	_src.convertTo(src_temp, CV_32F);

	#pragma omp parallel for collapse(2)
	for (int i = 0; i < src_temp.rows; i++)
	{
		for (int j = 0; j < src_temp.cols; j++)
		{
			if (src_temp.at<float>(i, j) == -INFINITY)
			{
				src_temp.at<float>(i, j) = 0;
			}
		}
	}

	double min, max;
	cv::minMaxLoc(src_temp, &min, &max);
	src_temp = src_temp - min;
	src_temp = src_temp / (max - min);
	//src_temp = src_temp / (max - min + 1);

	/************************************************************************************************************************************************************
	* Horizontal and Vertical Partial Derivatives (현재는 Input이 Depth Map인 것을 가정하므로, 1 Channel만 고려. RGB Image일 경우, 3 Channel 고려하여 합해야 함)
	* 여기서는 1 Channel만 고려하므로, Partial Derivative = L1 norm
	************************************************************************************************************************************************************/
	cv::Mat dIdx = cv::Mat::zeros(src_temp.rows, src_temp.cols, CV_32F);
	cv::Mat dIdy = cv::Mat::zeros(src_temp.rows, src_temp.cols, CV_32F);

	#pragma omp parallel for collapse(2)
	for (int i = 0; i < src_temp.rows; i++)
	{
		for (int j = 0; j < src_temp.cols; j++)
		{
			if (j + 1 < src_temp.cols)
			{
				dIdx.at<float>(i, j + 1) = abs(src_temp.at<float>(i, j + 1) - src_temp.at<float>(i, j));
			}
			if (i + 1 < src_temp.rows)
			{
				dIdy.at<float>(i + 1, j) = abs(src_temp.at<float>(i + 1, j) - src_temp.at<float>(i, j));
			}
		}
	}


	/***************************************************************************
	* Derivative of the Horizontal and Vertical Domain Transforms.
	* ct(u) = integral from 0 to u of (1 + sigma_s / sigma_r * abs(dI/dx)) du
	***************************************************************************/
	cv::Mat dHdx, dVdy;
	dHdx = (1 + _sigma_s / _sigma_r * dIdx);
	dVdy = (1 + _sigma_s / _sigma_r * dIdy);


	/**********************************
	* Integrate the Domain Transforms.
	**********************************/
	cv::Mat ct_H = cv::Mat::zeros(src_temp.rows, src_temp.cols, CV_32F);
	cv::Mat ct_V = cv::Mat::zeros(src_temp.rows, src_temp.cols, CV_32F);

	#pragma omp parallel sections
	{
		#pragma omp section
		{
			for (int i = 0; i < src_temp.rows; i++)
			{
				ct_H.at<float>(i, 0) = dHdx.at<float>(i, 0);
				for (int j = 1; j < src_temp.cols; j++)
				{
					ct_H.at<float>(i, j) = ct_H.at<float>(i, j - 1) + dHdx.at<float>(i, j);
				}
			}
		}
		#pragma omp section
		{
			for (int j = 0; j < src_temp.cols; j++)
			{
				ct_V.at<float>(0, j) = dVdy.at<float>(0, j);
				for (int i = 1; i < src_temp.rows; i++)
				{
					ct_V.at<float>(i, j) = ct_V.at<float>(i - 1, j) + dVdy.at<float>(i, j);
				}
			}
		}
	}
	cv::transpose(ct_V, ct_V);

	int sigma_H = _sigma_s;
	double sigma_H_i = _sigma_s;
	double box_radius = _sigma_s * sqrt(3);
	
	for (int i = 0; i < _num_iterations; i++)
	{
		sigma_H_i = sigma_H * sqrt(3) * pow(2, _num_iterations - (i + 1)) / sqrt(pow(4, _num_iterations) - 1);

		box_radius = sqrt(3) * sigma_H_i;

		TransformedDomainBoxFilter_Horizontal(src_temp, src_temp, ct_H, box_radius);
		cv::transpose(src_temp, src_temp);
		TransformedDomainBoxFilter_Horizontal(src_temp, src_temp, ct_V, box_radius);
		cv::transpose(src_temp, src_temp);
	}

	src_temp = src_temp * (max - min) + min;

	#pragma omp parallel for collapse (2)
	for (int i = 0; i < src_temp.rows; i++)
	{
		for (int j = 0; j < src_temp.cols; j++)
		{
			if (src_temp.at<float>(i, j) == 0)
			{
				src_temp.at<float>(i, j) = -INFINITY;
			}
		}
	}

	src_temp.convertTo(src_temp, _src.type());
	src_temp.copyTo(_dst);

	return true;
}


float GetSigmaH(float _sigma_H, int _num_iterations, int _curr_iteration)
{
	return _sigma_H * sqrt(3) * (pow(2, _num_iterations - (_curr_iteration + 1)) / sqrt(pow(4, _num_iterations) - 1));
}

void DomainTransform(cv::Mat _src, cv::Mat& _ct, float _sigma_s, float _sigma_r)
{
	// Domain Transform
	cv::Mat src_float;
	_src.convertTo(src_float, CV_32F);

	cv::normalize(src_float, src_float, 0, 1, cv::NORM_MINMAX); // 정규화하지 않으면 누적합 계산 시, 값이 너무 커져서 최대 범위를 넘어감

	cv::Mat dI_dx;
	Diff_Partial_X(src_float, dI_dx, DIFF_PARTIAL_OPENCV, DIFF_DIRECTION_RIGHT);

	_ct = 1 + (_sigma_s / _sigma_r) * cv::abs(dI_dx);
	for (int c = 1; c < src_float.cols; c++)
	{
		#pragma omp parallel for
		for (int r = 0; r < src_float.rows; r++)
		{
			_ct.at<float>(r, c) = _ct.at<float>(r, c) + _ct.at<float>(r, c - 1);
		}
	}
}

int FindLargerMin(cv::Mat _row, float _target, int _offset_min, int _offset_max)
{
	// 1차원 행렬에서 _target보다 큰 값 중 가장 작은 값의 인덱스를 0과 _offset 사이에서 찾음

	if (_offset_min == -1)
	{
		_offset_min = 0;
	}
	if (_offset_max == -1)
	{
		_offset_max = _row.cols - 1;
	}

	int min = _offset_min;
	int max = _offset_max;
	int curr;
	while (true)
	{
		if (min == max)
		{
			return min;
		}

		curr = min + std::round((max - min) / 2);
		if (_row.at<float>(curr) > _target)
		{
			if (curr == 0 || _row.at<float>(curr - 1) <= _target)
			{
				return curr;
			}
			else
			{
				max = curr - 1;
			}
		}
		else
		{
			min = curr + 1;
		}
	}

	return 0;
}

void NormalizedConvolution(cv::Mat _src, cv::Mat& _dst, cv::Mat _ct, float _box_radius)
{
	// x 방향 Normalized Convolution
	// _src: cv::MAt_<float>

	cv::Mat x_lower, x_upper;
	x_lower = _ct - _box_radius;
	x_upper = _ct + _box_radius;

	cv::Mat x_lower_idx, x_upper_idx;
	x_lower_idx = cv::Mat::zeros(_src.size(), CV_32S);
	x_upper_idx = cv::Mat::zeros(_src.size(), CV_32S);

	cv::Mat ct_row;
	#pragma omp parallel for collapse(2)
	for (int r = 0; r < _src.rows; r++)
	{
		ct_row = _ct.row(r);
		for (int c = 0; c < _src.cols; c++)
		{
			x_lower_idx.at<int>(r, c) = FindLargerMin(ct_row, x_lower.at<float>(r, c), c == 0 ? 0 : x_lower_idx.at<int>(r, c - 1), c);
			x_upper_idx.at<int>(r, c) = FindLargerMin(ct_row, x_upper.at<float>(r, c), c == 0 ? 0 : x_upper_idx.at<int>(r, c - 1), ct_row.cols - 1);
		}
	}

	cv::Mat accumulated_src;
	_src.copyTo(accumulated_src);
	#pragma omp parallel for
	for (int r = 0; r < _src.rows; r++)
	{
		for (int c = 1; c < _src.cols; c++)
		{
			accumulated_src.at<float>(r, c) += accumulated_src.at<float>(r, c - 1);
		}
	}

	accumulated_src.copyTo(_dst);
	int x_lower_idx_val, x_upper_idx_val;
	#pragma omp parallel for collapse(2)
	for (int r = 0; r < _src.rows; r++)
	{
		for (int c = 0; c < _src.cols; c++)
		{
			x_lower_idx_val = x_lower_idx.at<int>(r, c) = x_lower_idx.at<int>(r, c) - 1;
			x_upper_idx_val = x_upper_idx.at<int>(r, c) == _src.cols - 1 ? _src.cols - 1 : x_upper_idx.at<int>(r, c) - 1;

			if (x_lower_idx_val != x_upper_idx_val)
			{
				if (x_lower_idx_val < 0)
				{
					_dst.at<float>(r, c) = (accumulated_src.at<float>(r, x_upper_idx_val) - 0)
						/ (x_upper_idx_val - x_lower_idx_val);
				}
				else
				{
					_dst.at<float>(r, c) = (accumulated_src.at<float>(r, x_upper_idx_val) - accumulated_src.at<float>(r, x_lower_idx_val))
						/ (x_upper_idx_val - x_lower_idx_val);
				}
			}
		}
	}
}


void InterpolatedConvolution(cv::Mat _src, cv::Mat& _dst, cv::Mat _ct, float _box_radius)
{
	// x 방향 Interpolated Convolution
	// _src: cv::MAt_<float>

	cv::Mat x_lower, x_upper;
	x_lower = _ct - _box_radius;
	x_upper = _ct + _box_radius;

	cv::Mat x_lower_idx, x_upper_idx;
	x_lower_idx = cv::Mat::zeros(_src.size(), CV_32S);
	x_upper_idx = cv::Mat::zeros(_src.size(), CV_32S);

	cv::Mat ct_row;
	#pragma omp parallel for collapse(2)
	for (int r = 0; r < _src.rows; r++)
	{
		ct_row = _ct.row(r);
		for (int c = 0; c < _src.cols; c++)
		{
			x_lower_idx.at<int>(r, c) = FindLargerMin(ct_row, x_lower.at<float>(r, c), c == 0 ? 0 : x_lower_idx.at<int>(r, c - 1), c);
			x_upper_idx.at<int>(r, c) = FindLargerMin(ct_row, x_upper.at<float>(r, c), c == 0 ? 0 : x_upper_idx.at<int>(r, c - 1), ct_row.cols - 1);
		}
	}

	cv::Mat accumulated_src;
	_src.copyTo(accumulated_src);
	#pragma omp parallel for
	for (int r = 0; r < _src.rows; r++)
	{
		for (int c = 1; c < _src.cols; c++)
		{
			accumulated_src.at<float>(r, c) += accumulated_src.at<float>(r, c - 1);
		}
	}

	accumulated_src.copyTo(_dst);
	int x_lower_idx_val, x_upper_idx_val;
	#pragma omp parallel for collapse(2)
	for (int r = 0; r < _src.rows; r++)
	{
		for (int c = 0; c < _src.cols; c++)
		{
			x_lower_idx_val = x_lower_idx.at<int>(r, c) = x_lower_idx.at<int>(r, c) - 1;
			x_upper_idx_val = x_upper_idx.at<int>(r, c) == _src.cols - 1 ? _src.cols - 1 : x_upper_idx.at<int>(r, c) - 1;

			if (x_lower_idx_val != x_upper_idx_val)
			{
				if (x_lower_idx_val < 0)
				{
					_dst.at<float>(r, c) = (accumulated_src.at<float>(r, x_upper_idx_val) - 0)
						/ (x_upper_idx_val - x_lower_idx_val);
				}
				else
				{
					_dst.at<float>(r, c) = (accumulated_src.at<float>(r, x_upper_idx_val) - accumulated_src.at<float>(r, x_lower_idx_val))
						/ (x_upper_idx_val - x_lower_idx_val);
				}
			}
		}
	}
}


bool SpatialEdgePreservingFilter_v2(cv::Mat _src, cv::Mat& _dst, int _sigma_s, double _sigma_r, int _num_iterations, DOMAIN_TRANSFORM_MODE _mode)
{
	// _sigma_s와 _sigma_r을 합쳐서 하나의 파라미터로 만들어도 될 것 같음. 추후 검토
	TIMER();

	if (_mode == VERSION_1)
	{
		return false;
	}

	if (_src.empty() || _src.channels() > 1)
	{
		OutputDebugString(L"Invalid Input Image.");
		return false;
	}

	cv::Mat ct_x, ct_y;
	DomainTransform(_src, ct_x, _sigma_s, _sigma_r);
	DomainTransform(_src.t(), ct_y, _sigma_s, _sigma_r);

	cv::Mat src_float;
	_src.convertTo(src_float, CV_32F);
	float sigma_H = static_cast<float>(_sigma_s);
	float sigma_H_i, box_radius;
	for (int iter = 0; iter < _num_iterations; iter++)
	{
		sigma_H_i = GetSigmaH(sigma_H, _num_iterations, iter);
		box_radius = sqrt(3) * sigma_H_i;

		if (_mode == NORMALIZED_CONVOLUTION)
		{
			NormalizedConvolution(src_float, src_float, ct_x, box_radius);
			NormalizedConvolution(src_float.t(), src_float, ct_y, box_radius);
			cv::transpose(src_float, src_float);
		}
		else if (_mode == INTERPOLATED_CONVOLUTION)
		{
			OutputDebugString(L"Interpolated Convolution is not implemented yet.");
			return false;

			//InterpolatedConvolution(src_float, src_float, ct_x, box_radius);
			//InterpolatedConvolution(src_float.t(), src_float, ct_y, box_radius);
			//cv::transpose(src_float, src_float);
		}
		else if (_mode == RECURSIVE_FILTER)
		{
			OutputDebugString(L"Recursive Filter is not implemented yet.");
			return false;
		}
		else
		{
			OutputDebugString(L"Unsupported Domain Transform Mode for Spatial Edge Preserving Filter.");
			return false;
		}
	}

	src_float.convertTo(_dst, _src.type());

	return true;
}