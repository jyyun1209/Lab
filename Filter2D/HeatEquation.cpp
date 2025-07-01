#include "HeatEquation.h"

bool HeatEquationFilter(cv::Mat _src, cv::Mat& _dst, float _alpha, int _iterations)
{
	/* ------------------------------------------------
	 * Heat Equation: I_{t+1} = I_t + I_{xx] + I_{yy}
	--------------------------------------------------*/

	OutputDebugString(L"HeatEquationFilter called\n");

	if (_src.empty())
	{
		OutputDebugString(L"Input image is empty.\n");
		return false;
	}

	if (_src.channels() == 1)
	{
		cv::Mat laplacian_I;
		_src.convertTo(_dst, CV_32F);

		for (int iter = 0; iter < _iterations; iter++)
		{
			Laplacian(_dst, laplacian_I, LAPLACIAN_OPENCV);
			_dst = _dst + _alpha * laplacian_I;
		}
		_dst.convertTo(_dst, CV_8UC1);
	}
	else
	{
		OutputDebugString(L"Invalid image.\n");
		return false;
	}

	return true;
}