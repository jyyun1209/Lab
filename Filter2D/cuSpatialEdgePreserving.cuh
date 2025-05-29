#pragma once
#include <opencv2/opencv.hpp>

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "cuda_common.cuh"

#include "Filter2D_Exp.h"

extern "C"
{
	FILTER2D_DLL void cuSpatialEdgePreserving(cv::Mat h_inputImage, cv::Mat& h_outputImage, int _sigmaS, double _sigmaR, int _numIter = 3);
}