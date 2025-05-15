#include "MedianFilter.h"


int BorderReplicate_JY(int _p, int _len)
{
	if (_p < 0)
	{
		return 0;
	}
	else if (_p >= _len)
	{
		return _len - 1;
	}
	else
	{
		return _p;
	}
}

template <typename T>
T BorderInterpolate_JY(cv::Mat _src, int _row, int _col, int _ch, MODE_BORDER _mode)
{
	int r = 0;
	int c = 0;

	switch (_mode)
	{
	case MODE_BORDER::REPLICATE_CPP:
		r = BorderReplicate_JY(_row, _src.rows);
		c = BorderReplicate_JY(_col, _src.cols);
	}

	if (_src.channels() == 3)
	{
		return _src.at<cv::Vec<T, 3>>(r, c)[_ch];
	}
	else if (_src.channels() == 1)
	{
		return _src.at<cv::Vec<T, 1>>(r, c)[_ch];
	}
	else
	{
		return 0;
	}
}

bool MedianFilter2D(cv::Mat _src, cv::Mat& _dst, int _kx, int _ky, MODE_MEDIAN2D _mode)
{
	TIMER();
	//TIMER_CUDA();

	if (_kx == 0)
	{
		return 0;
	}

	if (_ky == -1)
	{
		_ky = _kx;
	}

	switch (_mode)
	{
	case MODE_MEDIAN2D::OPENCV:
	{
		cv::medianBlur(_src, _dst, _kx);
		return true;
	}
	//case MODE_MEDIAN2D_OPENCV_UMAT:
	//{
	//	cv::UMat src_umat, dst_umat;
	//	_src.copyTo(src_umat);
	//	cv::medianBlur(src_umat, dst_umat, _kx);
	//	dst_umat.copyTo(_dst);
	//	return true;
	//}
	//case MODE_MEDIAN2D_OPENCV_CUDA:
	//{
	//	// Src type: CV_8UC1¸¸ Áö¿ø
	//	cv::cuda::GpuMat src_gpu, dst_gpu;
	//	src_gpu.upload(_src);
	//	cv::Ptr<cv::cuda::Filter> median = cv::cuda::createMedianFilter(src_gpu.type(), _kx);
	//	median->apply(src_gpu, dst_gpu);
	//	return true;
	//}
	case MODE_MEDIAN2D::CUSTOM_CPP:
	{
		if (_src.type() == CV_8UC3 || _src.type() == CV_8UC1)
		{
			MedianFilter2D_CPP<uchar>(_src, _dst, _kx, _ky);
		}
		else if (_src.type() == CV_32FC1)
		{
			MedianFilter2D_CPP<float>(_src, _dst, _kx, _ky);
		}
		return true;
	}
	//case MODE_MEDIAN2D_CUSTOM_CUDA:
	//{
	//	cuMedianBlur(_src, _dst, _kx, _ky);
	//	return true;
	//}
	//case MODE_MEDIAN2D_CUSTOM_CPP_FINITE_ONLY:
	//{
	//	MedianFilter2D_CPP(_src, _dst, _kx, _ky, true);
	//	return true;
	//}
	//case MODE_MEDIAN2D_CUSTOM_CUDA_FINITE_ONLY:
	//{
	//	cuMedianBlur(_src, _dst, _kx, _ky, CUDA_BORDER_REPLICATE, true);
	//	return true;
	//}
	default:
		return false;
	}
}

template <typename T>
bool MedianFilter2D_CPP(cv::Mat _src, cv::Mat& _dst, int _kx, int _ky, bool _finiteOnly)
{
	int kw05 = _kx >> 1;
	int kh05 = _ky >> 1;
	cv::Mat temp = cv::Mat::zeros(_src.rows, _src.cols, _src.type());
	_src.copyTo(temp);

	#pragma	omp parallel for collapse (3)
	for (int ch = 0; ch < _src.channels(); ch++)
	{
		for (int row = 0; row < _src.rows; row++)
		{
			for (int col = 0; col < _src.cols; col++)
			{
				std::vector<T> kernel;
				T curr_val = 0;
				for (int i = -kh05; i <= kh05; i++)
				{
					for (int j = -kw05; j <= kw05; j++)
					{
						if (row - kh05 >= 0 && row + kh05 < _src.rows && col - kw05 >= 0 && col + kw05 < _src.cols)
						{
							if (_src.channels() == 3)
							{
								curr_val = _src.at<cv::Vec<T, 3>>(row + i, col + j)[ch];
							}
							else if (_src.channels() == 1)
							{
								curr_val = _src.at<cv::Vec<T, 1>>(row + i, col + j)[ch];
							}
							else
							{
								// Do nothing
							}
						}
						else
						{
							curr_val = BorderInterpolate_JY<T>(_src, row + i, col + j, ch, MODE_BORDER::REPLICATE_CPP);
						}
						//if (_finiteOnly)
						//{
						//	if (isfinite(curr_val))
						//	{
						//		kernel.push_back(curr_val);
						//	}
						//}
						//else
						//{
						//	kernel.push_back(curr_val);
						//}
						kernel.push_back(curr_val);
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
					if (_src.channels() == 3)
					{
						temp.at<cv::Vec<T, 3>>(row, col)[ch] = kernel[kernel.size() / 2];
					}
					else if (_src.channels() == 1)
					{
						temp.at<cv::Vec<T, 1>>(row, col)[ch] = kernel[kernel.size() / 2];
					}
					else
					{
						// Do nothing
					}
				}
				else
				{
					if (_src.channels() == 3)
					{
						temp.at<cv::Vec<T, 3>>(row, col)[ch] = _src.at<cv::Vec<T, 3>>(row, col)[ch];
					}
					else if (_src.channels() == 1)
					{
						temp.at<cv::Vec<T, 1>>(row, col)[ch] = _src.at<cv::Vec<T, 1>>(row, col)[ch];
					}
					else
					{
						// Do nothing
					}
				}
			}
		}
	}

	temp.copyTo(_dst);
	return true;
}