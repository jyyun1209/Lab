#include "ReconstructionCore.h"

bool LoadCalibData(std::string _filePath, Reconstruction::CalibrationParams& _calibParams)
{
	cv::FileStorage fs(_filePath, cv::FileStorage::READ);
	if (!fs.isOpened())
	{
		OutputDebugString(L"File Open Failed\n");
		fs.release();
		return false;
	}
	
	fs["Left_CAM_K"] >> _calibParams.cameraMatrixL;
	fs["Left_CAM_D"] >> _calibParams.distCoeffsL;
	fs["Right_CAM_K"] >> _calibParams.cameraMatrixR;
	fs["Right_CAM_D"] >> _calibParams.distCoeffsR;
	fs["R"] >> _calibParams.R;
	fs["T"] >> _calibParams.T;
	fs["Essential Mat"] >> _calibParams.essentialMatrix;
	fs["Fundamental Mat"] >> _calibParams.fundamentalMatrix;
	fs["R_rect_left"] >> _calibParams.R_rectL;
	fs["R_rect_right"] >> _calibParams.R_rectR;
	fs["P_rect_left"] >> _calibParams.P_rectL;
	fs["P_rect_right"] >> _calibParams.P_rectR;
	fs["Q"] >> _calibParams.Q;

	fs.release();

	return true;
}

void DepthToPoint(const cv::Mat& _depthmap, cv::Mat& _points, const cv::Mat& _K)
{
	TIMER();

	_points = cv::Mat::zeros(_depthmap.size(), CV_32FC3);

	const double& fx = _K.ptr<double>(0)[0];
	const double& fy = _K.ptr<double>(1)[1];
	const double& cx = _K.ptr<double>(0)[2];
	const double& cy = _K.ptr<double>(1)[2];
	// Eigen::Matrix4d camera_pose = extrinsic.inverse();

	int width = _depthmap.cols;
	int height = _depthmap.rows;
	float x, y, z;

	_points = -INFINITY;

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			const float* p = _depthmap.ptr<float>(j, i);
			if (::isnan(*p) || ::isinf(*p))
				continue;

			z = (*p);
			x = (i - cx) * z / fx;
			y = (j - cy) * z / fy;
			// Eigen::Vector4d point = camera_pose * Eigen::Vector4d(x, y, z, 1.0);
			cv::Vec3f* pt = _points.ptr<cv::Vec3f>(j, i);
			pt->val[0] = x;
			pt->val[1] = y;
			pt->val[2] = z;
		}
	}
}