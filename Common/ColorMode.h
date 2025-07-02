#pragma once

#include <opencv2/opencv.hpp>
#include "Common_Exp.h"

extern "C"
{
	COMMON_DLL void Color2Grayscale(cv::Mat _src, cv::Mat& _dst);
}