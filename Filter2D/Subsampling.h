#pragma once

#include <opencv2/opencv.hpp>
#include "Filter2D_Exp.h"
#include "Common/Define.h"

extern "C" FILTER2D_DLL bool Subsampling(cv::Mat _src, cv::Mat& _dst, SUBSAMPLE_MODE _mode, SUBSAMPLE_INTERPOLATION_MODE _inter_mode);