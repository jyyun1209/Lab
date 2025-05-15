#pragma once
#include <opencv2/opencv.hpp>

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "cuda_common.cuh"

#include "Filter2D_Exp.h"

enum cuBorderTypes 
{
	CUDA_BORDER_REPLICATE = 0,
};

__device__ void d_insertionSort(float* arr, int size);
__device__ int borderReplicate(int idx, int size);
__device__ float d_borderInterpolate(float* d_inputImage, int x, int y, int width, int height, int borderType);
__global__ void d_medianBlur(float* d_inputImage, float* d_outputImage, int width, int height, int kernel_x, int kernel_y, int borderType, bool finiteOnly);

extern "C"
{
	FILTER2D_DLL void cuMedianBlur(cv::Mat h_inputImage, cv::Mat& h_outputImage, int kernel_x, int kernel_y, int borderType = CUDA_BORDER_REPLICATE, bool finiteOnly = false);
}