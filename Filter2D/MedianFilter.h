#pragma once

#include <opencv2/opencv.hpp>

#define FILTER2D_DLL __declspec(dllexport)

enum class MODE_MEDIAN2D
{
	OPENCV = 0,
	OPENCV_UMAT,
	OPENCV_CUDA,
	CUSTOM_CPP,
	CUSTOM_CUDA,
	CUSTOM_CPP_FINITE_ONLY,
	CUSTOM_CUDA_FINITE_ONLY,
};

enum class MODE_BORDER
{
	REPLICATE_CPP = 0,
};



int BorderReplicate_JY(int _p, int _len);
float BorderInterpolate_JY(cv::Mat _src, int _row, int _col, MODE_BORDER _mode);
bool MedianFilter2D_CPP(cv::Mat _src, cv::Mat& _dst, int _kx, int _ky, bool _finiteOnly=false);
extern "C" FILTER2D_DLL bool MedianFilter2D(cv::Mat _src, cv::Mat& _dst, int _kx, int _ky=-1, MODE_MEDIAN2D _mode=MODE_MEDIAN2D::OPENCV);