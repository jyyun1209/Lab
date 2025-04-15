#include "MedianFilter.h"

bool MedianFilter(cv::Mat _src, cv::Mat _dst, int _k)
{
	if (_k == 0)
	{
		return true;
	}
	else
	{
		cv::medianBlur(_src, _dst, _k);
	}

	return true;
}

#if 0
bool MedianFilter2D(cv::Mat _src, cv::Mat& _dst, int _kx, int _ky, MODE_MEDIAN2D _mode)
{
	TIMER();
	TIMER_CUDA();

	if (_ky == -1)
	{
		_ky = _kx;
	}

	switch (_mode)
	{
	case MODE_MEDIAN2D_OPENCV:
	{
		cv::medianBlur(_src, _dst, _kx);
		return true;
	}
	case MODE_MEDIAN2D_OPENCV_UMAT:
	{
		cv::UMat src_umat, dst_umat;
		_src.copyTo(src_umat);
		cv::medianBlur(src_umat, dst_umat, _kx);
		dst_umat.copyTo(_dst);
		return true;
	}
	case MODE_MEDIAN2D_OPENCV_CUDA:
	{
		// Src type: CV_8UC1만 지원
		cv::cuda::GpuMat src_gpu, dst_gpu;
		src_gpu.upload(_src);
		cv::Ptr<cv::cuda::Filter> median = cv::cuda::createMedianFilter(src_gpu.type(), _kx);
		median->apply(src_gpu, dst_gpu);
		return true;
	}
	case MODE_MEDIAN2D_CUSTOM_CPP:
	{
		MedianFilter2D_CPP(_src, _dst, _kx, _ky);
		return true;
	}
	case MODE_MEDIAN2D_CUSTOM_CUDA:
	{
		cuMedianBlur(_src, _dst, _kx, _ky);
		return true;
	}
	case MODE_MEDIAN2D_CUSTOM_CPP_FINITE_ONLY:
	{
		MedianFilter2D_CPP(_src, _dst, _kx, _ky, true);
		return true;
	}
	case MODE_MEDIAN2D_CUSTOM_CUDA_FINITE_ONLY:
	{
		cuMedianBlur(_src, _dst, _kx, _ky, CUDA_BORDER_REPLICATE, true);
		return true;
	}
	default:
		return false;
	}
}

bool MedianFilter2D_CPP(cv::Mat _src, cv::Mat& _dst, int _kx, int _ky, bool _finiteOnly)
{
	int kw05 = _kx >> 1;
	int kh05 = _ky >> 1;
	cv::Mat temp = cv::Mat::zeros(_src.rows, _src.cols, CV_32F);
	_src.copyTo(temp);

#pragma	omp parallel for collapse(2)
	for (int row = 0; row < _src.rows; row++)
	{
		for (int col = 0; col < _src.cols; col++)
		{
			std::vector<float> kernel;
			float curr_val = 0;
			for (int i = -kh05; i <= kh05; i++)
			{
				for (int j = -kw05; j <= kw05; j++)
				{
					if (row - kh05 >= 0 && row + kh05 < _src.rows && col - kw05 >= 0 && col + kw05 < _src.cols)
					{
						curr_val = _src.at<float>(row + i, col + j);
					}
					else
					{
						curr_val = borderInterpolate_LP(_src, row + i, col + j, BORDER_REPLICATE_CPP);
					}
					if (_finiteOnly)
					{
						if (isfinite(curr_val))
						{
							kernel.push_back(curr_val);
						}
					}
					else
					{
						kernel.push_back(curr_val);
					}
				}
			}

			int threshold = 0;
			if (_finiteOnly)
			{
				threshold = (_kx * _ky) / 2;
			}

			if (kernel.size() > threshold)
			{
				std::nth_element(kernel.begin(), kernel.begin() + kernel.size() / 2, kernel.end());
				temp.at<float>(row, col) = kernel[kernel.size() / 2];
			}
			else
			{
				temp.at<float>(row, col) = _src.at<float>(row, col);
			}
		}
	}

	temp.copyTo(_dst);
	return true;
}
#endif

#if 0
bool TransformedDomainBoxFilter_Horizontal(cv::Mat src, cv::Mat dst, cv::Mat ct, double box_radius)
{
	/***********************************************************************************************
	* Normalized Convolution
	* Interpolated Convolution, Recursive Filter는 추후 구현 필요
	***********************************************************************************************/
	cv::Mat temp = cv::Mat::zeros(src.rows, src.cols, CV_32F);
	cv::Mat SAT = cv::Mat::zeros(src.rows, src.cols + 1, CV_32F);

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

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 1; j <= src.cols; j++)
		{
			SAT.at<float>(i, j) = SAT.at<float>(i, j) + SAT.at<float>(i, j - 1);
		}
	}

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

bool SpatialEdgePreservingFilter(cv::Mat _src, cv::Mat& _dst, int _sigma_s, double _sigma_r, int _num_iterations)
{
	TIMER();
	TIMER_CUDA();

	/******************
	* Domain Transform
	******************/
	cv::Mat src_temp = cv::Mat::zeros(_src.rows, _src.cols, CV_32F);
	for (int i = 0; i < _src.rows; i++)
	{
		for (int j = 0; j < _src.cols; j++)
		{
			//if (src.at<uint8_t>(i, j) == -INFINITY)
			if (_src.at<float>(i, j) == -INFINITY)
			{
				src_temp.at<float>(i, j) = 0;
			}
			else
			{
				//src_temp.at<float>(i, j) = src.at<uint8_t>(i, j);
				src_temp.at<float>(i, j) = _src.at<float>(i, j);
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
	for (int i = 0; i < src_temp.rows; i++)
	{
		ct_H.at<float>(i, 0) = dHdx.at<float>(i, 0);
		for (int j = 1; j < src_temp.cols; j++)
		{
			ct_H.at<float>(i, j) = ct_H.at<float>(i, j - 1) + dHdx.at<float>(i, j);
		}
	}
	for (int j = 0; j < src_temp.cols; j++)
	{
		ct_V.at<float>(0, j) = dVdy.at<float>(0, j);
		for (int i = 1; i < src_temp.rows; i++)
		{
			ct_V.at<float>(i, j) = ct_V.at<float>(i - 1, j) + dVdy.at<float>(i, j);
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
	src_temp.copyTo(_dst);
	return true;
}
#endif