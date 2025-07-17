#include "GaussianFilter.h"

bool GaussianFilter(cv::Mat _src, cv::Mat& _dst, int _ksize, float _sigma)
{
	cv::GaussianBlur(_src, _dst, cv::Size(_ksize, _ksize), _sigma, _sigma, cv::BORDER_DEFAULT);

	return true;
}