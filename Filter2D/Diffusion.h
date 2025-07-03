#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/ximgproc.hpp>
#include "Filter2D_Exp.h"
#include "Common/Differentiation.h"
#include "Windows.h"
#include "Common/Timer.h"

enum DIFFUSION_COEFFICIENT_MODE
{
	DIFFUSION_COEFFICIENT_EXPONENTIAL = 0,
	DIFFUSION_COEFFICIENT_RECIPROCAL,
};

float AnisotropicDiffusion_Coefficient(float grad_I, float _k, DIFFUSION_COEFFICIENT_MODE _mode = DIFFUSION_COEFFICIENT_RECIPROCAL);
void AnisotropicDiffusion_Coefficient_Mat(cv::Mat _grad_I, cv::Mat& _ret, float _k, DIFFUSION_COEFFICIENT_MODE _mode = DIFFUSION_COEFFICIENT_RECIPROCAL);

bool AnisotropicDiffusion_OpenCV(cv::Mat _src, cv::Mat& _dst, float _k, int _iter);
bool AnisotropicDiffusion_CPP(cv::Mat _src, cv::Mat& _dst, float _k, int _iter, DIFFUSION_COEFFICIENT_MODE _mode = DIFFUSION_COEFFICIENT_RECIPROCAL);
bool AnisotropicDiffusion_CPP_v2(cv::Mat _src, cv::Mat& _dst, float _k, int _iter, DIFFUSION_COEFFICIENT_MODE _mode = DIFFUSION_COEFFICIENT_RECIPROCAL);


extern "C"
{
	FILTER2D_DLL bool HeatEquationFilter(cv::Mat _src, cv::Mat& _dst, float _alpha = 0.1, int _iterations = 3);
	FILTER2D_DLL bool AnisotropicDiffusion(cv::Mat _src, cv::Mat& _dst, float _k, int _iter, ANISOTROPIC_DIFFUSION_MODE _mode = ANISOTROPIC_DIFFUSION_OPENCV);
}