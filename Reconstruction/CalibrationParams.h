#pragma once

#include <string>
#include <vector>
#include <map>
#include <opencv2/opencv.hpp>

namespace Reconstruction
{
	struct CalibrationParams
	{
		cv::Mat cameraMatrixL, distCoeffsL, R_L, T_L,
			cameraMatrixR, distCoeffsR, R_R, T_R,
			cameraMatrixStereo, R, T, essentialMatrix, fundamentalMatrix,
			R_rectL, R_rectR, P_rectL, P_rectR, Q,
			R_L_rect, R_R_rect,
			calHeight;
		cv::Rect validRoI[2];
		cv::Mat map[2][2];
		double rmsL, rmsR, rms;
	};
}