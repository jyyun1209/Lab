#pragma once

#include <opencv2/opencv.hpp>
#include "Common_Exp.h"

extern "C"
{
	COMMON_DLL void Color2Grayscale(cv::Mat _src, cv::Mat& _dst);
	COMMON_DLL void RGBGain(cv::Mat _src, cv::Mat& _dst, float _rGain, float _gGain, float _bGain);
}