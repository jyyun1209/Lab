#pragma once

#include <opencv2/opencv.hpp>
#include <ctime>
#include "Common_Exp.h"


void Convert32FTo16U(const cv::Mat& _depth32, cv::Mat& _depth16, const float& _depth_scale = 1.0f, const float& _depth_offset = 0.0f);

extern "C"
{
	COMMON_DLL void LoadImageToCV(cv::Mat& _image, std::string _path);
	COMMON_DLL void SaveImage(cv::Mat image, std::string saveLocation, std::string _ext);
}