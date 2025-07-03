#pragma once

#include <Windows.h>
#include <string>
#include <opencv2/core.hpp>
#include "Reconstruction_Exp.h"
#include "CalibrationParams.h"
#include "Common/Timer.h"

extern "C"
{
	RECONSTRUCTION_DLL bool LoadCalibData(std::string _filePath, Reconstruction::CalibrationParams& _calibParams);
	RECONSTRUCTION_DLL void DepthToPoint(const cv::Mat& _depthmap, cv::Mat& _points, const cv::Mat& _K);
}