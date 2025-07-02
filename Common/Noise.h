#pragma once
#include <opencv2/opencv.hpp>
#include "Common_Exp.h"

extern"C"
{
	COMMON_DLL void SaltAndPepper(cv::Mat _src, cv::Mat& _dst, int _strength);
}