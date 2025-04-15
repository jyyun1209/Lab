#pragma once
#include <opencv2/opencv.hpp>

#include "TkCommonExport.h"

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "cuda_common.cuh"
#include "typeDefinition.h"
#include "math_constants.h"

/* Insertion Sort */
extern "C" LPTKCOMMON_EXP __device__ void d_insertionSort(float* arr, int size);

/* Median Filter */
extern "C" LPTKCOMMON_EXP __device__ int borderReplicate(int idx, int size);
extern "C" LPTKCOMMON_EXP __device__ float d_borderInterpolate(float* d_inputImage, int x, int y, int width, int height, int borderType);
extern "C" LPTKCOMMON_EXP __global__ void d_medianBlur(float* d_inputImage, float* d_outputImage, int width, int height, int kernel_x, int kernel_y, int borderType, bool finiteOnly);
extern "C" LPTKCOMMON_EXP void cuMedianBlur(cv::Mat h_inputImage, cv::Mat& h_outputImage, int kernel_x, int kernel_y, int borderType = CUDA_BORDER_REPLICATE, bool finiteOnly = false);

/* Real-Time DPC */
extern "C" LPTKCOMMON_EXP __global__ void d_rtDPC(float* d_inputImage, float* d_outputImage, int width, int height, int kernel_x, int kernel_y, float threshold, int borderType);
extern "C" LPTKCOMMON_EXP void cuRTDPC(cv::Mat h_inputImage, cv::Mat& h_outputImage, int kernel_x, int kernel_y, float threshold, int borderType = CUDA_BORDER_REPLICATE);