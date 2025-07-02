#pragma once

#include <opencv2/opencv.hpp>
#include "Filter2D_Exp.h"
#include "Common/Differentiation.h"
#include "Windows.h"

enum DIFFUSION_COEFFICIENT_MODE
{
	DIFFUSION_COEFFICIENT_EXPONENTIAL = 0,
	DIFFUSION_COEFFICIENT_RECIPROCAL,
};

float AnisotropicDiffusion_Coefficient(float grad_I, float _k, DIFFUSION_COEFFICIENT_MODE _mode = DIFFUSION_COEFFICIENT_RECIPROCAL);

extern "C" FILTER2D_DLL bool HeatEquationFilter(cv::Mat _src, cv::Mat& _dst, float _alpha = 0.1, int _iterations = 3);
extern "C" FILTER2D_DLL bool AnisotropicDiffusion(cv::Mat _src, cv::Mat& _dst, float _k, int _iter, DIFFUSION_COEFFICIENT_MODE _mode = DIFFUSION_COEFFICIENT_RECIPROCAL);