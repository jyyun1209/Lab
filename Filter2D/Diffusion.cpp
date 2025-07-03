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

void AnisotropicDiffusion_Coefficient_Mat(cv::Mat _grad_I, cv::Mat& _ret, float _k, DIFFUSION_COEFFICIENT_MODE _mode)
{
	switch (_mode)
	{
	case DIFFUSION_COEFFICIENT_RECIPROCAL:
		_ret = _grad_I.clone();
		_ret = cv::abs(_ret);
		_ret = _ret / _k;
		cv::pow(_ret, 2, _ret);
		_ret = _ret + 1.0f;
		_ret = 1.0f / _ret;
		break;
	}
}

bool AnisotropicDiffusion_OpenCV(cv::Mat _src, cv::Mat& _dst, float _k, int _iter)
{
	TIMER();

	if (_src.empty() || _src.channels() != 3)
	{
		OutputDebugString(L"Input image is empty or not a 3-channel image.\n");
		_dst = _src.clone();
		return false;
	}
	cv::ximgproc::anisotropicDiffusion(_src, _dst, 1.0f, _k, _iter);

	return true;
}

bool AnisotropicDiffusion_CPP(cv::Mat _src, cv::Mat& _dst, float _k, int _iter, DIFFUSION_COEFFICIENT_MODE _mode)
{
	/* ---------------------------------------------------------------
	 * Anisotropic Diffusion: I_{t+1} = I_t + c * (I_{xx] + I_{yy})
	----------------------------------------------------------------*/

	//OutputDebugString(L"AnisotropicDiffusion called\n");
	TIMER();

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

bool AnisotropicDiffusion_CPP_v2(cv::Mat _src, cv::Mat& _dst, float _k, int _iter, DIFFUSION_COEFFICIENT_MODE _mode)
{
	/* ---------------------------------------------------------------
	 * Anisotropic Diffusion: I_{t+1} = I_t + c * (I_{xx] + I_{yy})
	----------------------------------------------------------------*/

	//OutputDebugString(L"AnisotropicDiffusion called\n");
	TIMER();

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

			cv::Mat c_N, c_E, c_W, c_S;
			AnisotropicDiffusion_Coefficient_Mat(grad_N, c_N, _k, _mode);
			AnisotropicDiffusion_Coefficient_Mat(grad_E, c_E, _k, _mode);
			AnisotropicDiffusion_Coefficient_Mat(grad_W, c_W, _k, _mode);
			AnisotropicDiffusion_Coefficient_Mat(grad_S, c_S, _k, _mode);

			#pragma omp parallel for collapse(2)
			for (int r = 0; r < _src.rows; r++)
			{
				for (int c = 0; c < _src.cols; c++)
				{
					_dst.at<float>(r, c) += c_N.at<float>(r, c) * grad_N.at<float>(r, c) + c_E.at<float>(r, c) * grad_E.at<float>(r, c) +
											c_W.at<float>(r, c) * grad_W.at<float>(r, c) + c_S.at<float>(r, c) * grad_S.at<float>(r, c);
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

bool AnisotropicDiffusion(cv::Mat _src, cv::Mat& _dst, float _k, int _iter, ANISOTROPIC_DIFFUSION_MODE _mode)
{
	switch (_mode)
	{
	case (ANISOTROPIC_DIFFUSION_OPENCV):
		AnisotropicDiffusion_OpenCV(_src, _dst, _k, _iter);
		break;
		
	case (ANISOTROPIC_DIFFUSION_CPP):
		AnisotropicDiffusion_CPP(_src, _dst, _k, _iter, DIFFUSION_COEFFICIENT_RECIPROCAL);
		break;

	case (ANISOTROPIC_DIFFUSION_CPP_V2):
		AnisotropicDiffusion_CPP_v2(_src, _dst, _k, _iter, DIFFUSION_COEFFICIENT_RECIPROCAL);
		break;

	default:
		OutputDebugString(L"Invalid Anisotropic Diffusion Mode.\n");
		return false;
	}

	return true;
}