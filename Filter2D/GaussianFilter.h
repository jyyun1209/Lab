#pragma once

#include <opencv2/opencv.hpp>
#include "Filter2D_Exp.h"

extern "C" FILTER2D_DLL bool GaussianFilter(cv::Mat _src, cv::Mat& _dst, int _ksize, float _sigma);