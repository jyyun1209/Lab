#pragma once

#include <opencv2/opencv.hpp>
#include "Common_Exp.h"
#include "Windows.h"

extern "C"
{
	COMMON_DLL void Diff_Partial_X(cv::Mat _src, cv::Mat& _dst);
	COMMON_DLL void Diff_Partial_Y(cv::Mat _src, cv::Mat& _dst);
}