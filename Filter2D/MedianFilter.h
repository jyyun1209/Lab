#pragma once

#include <opencv2/opencv.hpp>

#define FILTER2D_DLL __declspec(dllexport)

extern "C" FILTER2D_DLL bool MedianFilter(cv::Mat _src, cv::Mat _dst, int _k);