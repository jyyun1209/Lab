#pragma once

#include <opencv2/opencv.hpp>
#include "Common_Exp.h"
#include "Define.h"
#include "Windows.h"

void Diff_Partial_X_OpenCV(cv::Mat _src, cv::Mat& _dst);
void Diff_Partial_X_CPP(cv::Mat _src, cv::Mat& _dst);

void Laplacian_OpenCV(cv::Mat _src, cv::Mat& _dst);
void Laplacian_CPP(cv::Mat _src, cv::Mat& _dst);

extern "C"
{
	COMMON_DLL void Diff_Partial_X(cv::Mat _src, cv::Mat& _dst, DIFF_PARTIAL_MODE _mode = DIFF_PARTIAL_OPENCV);
	COMMON_DLL void Diff_Partial_Y(cv::Mat _src, cv::Mat& _dst, DIFF_PARTIAL_MODE _mode = DIFF_PARTIAL_OPENCV);

	COMMON_DLL void Laplacian(cv::Mat _src, cv::Mat& _dst, LAPLACIAN_MODE _mode = LAPLACIAN_OPENCV);
}