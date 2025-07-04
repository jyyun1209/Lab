#pragma once

#include <opencv2/opencv.hpp>
#include "Filter2D_Exp.h"
#include "Common/Timer.h"
#include "Common/Define.h"

#include "cuMedianFilter.cuh"


enum class MODE_BORDER
{
	REPLICATE_CPP = 0,
};



int BorderReplicate_JY(int _p, int _len);
template <typename T> T BorderInterpolate_JY(cv::Mat _src, int _row, int _col, int _ch, MODE_BORDER _mode);
template <typename T> bool MedianFilter2D_CPP(cv::Mat _src, cv::Mat& _dst, int _kx, int _ky, bool _finiteOnly=false);

extern "C"
{
	FILTER2D_DLL bool MedianFilter2D(cv::Mat _src, cv::Mat& _dst, int _kx, int _ky = -1, MODE_MEDIAN2D _mode = MODE_MEDIAN2D::OPENCV);
}