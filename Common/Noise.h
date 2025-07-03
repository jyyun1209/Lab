#pragma once
#include <opencv2/opencv.hpp>
#include <random>
#include "Common_Exp.h"

extern"C"
{
	COMMON_DLL void SaltAndPepper(cv::Mat _src, cv::Mat& _dst, int _strength);
	COMMON_DLL void SpeckleNoise(cv::Mat _src, cv::Mat& _dst, float _stddev = 0.1f);
	COMMON_DLL void GaussianNoise(cv::Mat _src, cv::Mat& _dst, float _stddev = 0.1f);
}