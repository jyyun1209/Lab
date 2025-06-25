#pragma once

#include <opencv2/opencv.hpp>
#include "Filter2D_Exp.h"
#include "Windows.h"


extern "C" FILTER2D_DLL bool HeatEquationFilter(cv::Mat _src, cv::Mat& _dst, float _alpha = 0.1, int _iterations = 3);