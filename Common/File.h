#pragma once

#include <opencv2/opencv.hpp>
#include "Common_Exp.h"

void Convert32FTo16U(const cv::Mat& _depth32, cv::Mat& _depth16, const float& _depth_scale, const float& _depth_offset);

extern "C" COMMON_DLL void saveImage(cv::Mat image, std::string saveLocation, float scale = 1);