#pragma once

#include <opencv2/opencv.hpp>
#include "Filter2D_Exp.h"
#include "Common/Timer.h"

int largerMin(cv::Mat src, float value, int offset = 0);
bool TransformedDomainBoxFilter_Horizontal(cv::Mat src, cv::Mat dst, cv::Mat ct, double box_radius);

extern "C" FILTER2D_DLL bool SpatialEdgePreservingFilter(cv::Mat _src, cv::Mat& _dst, int _sigma_s, double _sigma_r, int _num_iterations = 3);