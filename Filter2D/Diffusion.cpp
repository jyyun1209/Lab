#include "Diffusion.h"

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


float AnisotropicDiffusion_Coefficient(float grad_I, float _k, DIFFUSION_COEFFICIENT_MODE _mode)
{
	float c;

	switch (_mode)
	{
	case DIFFUSION_COEFFICIENT_RECIPROCAL:
		c = 1.0f / (pow((abs(grad_I) / _k), 2) + 1.0f);
		break;
	}

	return c;
}

bool AnisotropicDiffusion(cv::Mat _src, cv::Mat& _dst, float _k, int _iter, DIFFUSION_COEFFICIENT_MODE _mode)
{
	/* ---------------------------------------------------------------
	 * Anisotropic Diffusion: I_{t+1} = I_t + c * (I_{xx] + I_{yy})
	----------------------------------------------------------------*/

	OutputDebugString(L"AnisotropicDiffusion called\n");

	if (_src.empty())
	{
		OutputDebugString(L"Input image is empty.\n");
		return false;
	}

	if (_src.channels() == 1)
	{
		_src.convertTo(_dst, CV_32F);


		for (int iter = 0; iter < _iter; iter++)
		{
			cv::Mat _dst_flip_vertical, _dst_flip_horizontal;
			cv::flip(_dst, _dst_flip_vertical, 0); // 상하 반전
			cv::flip(_dst, _dst_flip_horizontal, 1); // 좌우 반전

			cv::Mat grad_N, grad_E, grad_W, grad_S;
			Diff_Partial_Y(_dst_flip_vertical, grad_N, DIFF_PARTIAL_OPENCV, DIFF_DIRECTION_LEFT);
			cv::flip(grad_N, grad_N, 0);
			Diff_Partial_X(_dst, grad_E, DIFF_PARTIAL_OPENCV, DIFF_DIRECTION_LEFT);
			Diff_Partial_X(_dst_flip_horizontal, grad_W, DIFF_PARTIAL_OPENCV, DIFF_DIRECTION_LEFT);
			cv::flip(grad_W, grad_W, 1);
			Diff_Partial_Y(_dst, grad_S, DIFF_PARTIAL_OPENCV, DIFF_DIRECTION_LEFT);

			float c_N, c_E, c_W, c_S;
			#pragma omp parallel for collapse(2)
			for (int r = 0; r < _src.rows; r++)
			{
				for (int c = 0; c < _src.cols; c++)
				{
					c_N = AnisotropicDiffusion_Coefficient(grad_N.at<float>(r, c), _k, _mode);
					c_E = AnisotropicDiffusion_Coefficient(grad_E.at<float>(r, c), _k, _mode);
					c_W = AnisotropicDiffusion_Coefficient(grad_W.at<float>(r, c), _k, _mode);
					c_S = AnisotropicDiffusion_Coefficient(grad_S.at<float>(r, c), _k, _mode);

					_dst.at<float>(r, c) += c_N * grad_N.at<float>(r, c) + c_E * grad_E.at<float>(r, c) +
						c_W * grad_W.at<float>(r, c) + c_S * grad_S.at<float>(r, c);
				}
			}
		}

		_dst.convertTo(_dst, _src.type());
	}
	else
	{
		OutputDebugString(L"Invalid image.\n");
		return false;
	}

	return true;
}