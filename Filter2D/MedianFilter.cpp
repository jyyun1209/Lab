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