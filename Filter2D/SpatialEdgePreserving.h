#pragma once

#include <opencv2/opencv.hpp>
#include "Filter2D_Exp.h"
#include "Common/Timer.h"
#include "Common/File.h"
#include "Common/Define.h"
#include "Common/Differentiation.h"

int largerMin(cv::Mat src, float value, int offset = 0);
bool TransformedDomainBoxFilter_Horizontal(cv::Mat src, cv::Mat& dst, cv::Mat ct, double box_radius);
bool SpatialEdgePreservingFilter_v1(cv::Mat _src, cv::Mat& _dst, int _sigma_s, double _sigma_r, int _num_iterations = 3);

float GetSigmaH(float _sigma_H, int _num_iterations, int _curr_iteration);
void DomainTransform(cv::Mat _src, cv::Mat& _ct, float _sigma_s, float _sigma_r);
int FindLargerMin(cv::Mat _row, float _target, int _offset_min, int _offset_max);
void NormalizedConvolution(cv::Mat _src, cv::Mat& _dst, cv::Mat _ct, float _box_radius);
bool SpatialEdgePreservingFilter_v2(cv::Mat _src, cv::Mat& _dst, int _sigma_s, double _sigma_r, int _num_iterations = 3);


extern "C" FILTER2D_DLL bool DomainTransformFilter(cv::Mat _src, cv::Mat& _dst, int _sigma_s, double _sigma_r, int _num_iterations = 3, DOMAIN_TRANSFORM_MODE _mode = DOMAIN_TRANSFORM_V1);