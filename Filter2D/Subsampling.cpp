#include "Subsampling.h"

bool Subsampling(cv::Mat _src, cv::Mat& _dst, SUBSAMPLE_MODE _mode, SUBSAMPLE_INTERPOLATION_MODE _inter_mode)
{
	cv::InterpolationFlags interpolation_flag;

	switch (_inter_mode)
	{
	case SUBSAMPLE_INTERPOLATION_NEAREST:
		interpolation_flag = cv::INTER_NEAREST;
		break;

	case SUBSAMPLE_INTERPOLATION_LINEAR:
		interpolation_flag = cv::INTER_LINEAR;
		break;

	case SUBSAMPLE_INTERPOLATION_AREA:
		interpolation_flag = cv::INTER_AREA;
		break;

	case SUBSAMPLE_INTERPOLATION_CUBIC:
		interpolation_flag = cv::INTER_CUBIC;
		break;

	default:
		interpolation_flag = cv::INTER_NEAREST;
	}

	switch (_mode)
	{
	case SUBSAMPLE_2X2:
		cv::resize(_src, _dst, cv::Size(), 0.5, 0.5, interpolation_flag);
		break;

	case SUBSAMPLE_2X2_WITH_GAUSSIAN:
		cv::GaussianBlur(_src, _src, cv::Size(3, 3), 0.8, 0.8, cv::BORDER_DEFAULT);
		cv::resize(_src, _dst, cv::Size(), 0.5, 0.5, interpolation_flag);
		break;

	case SUBSAMPLE_4X4:
		cv::resize(_src, _dst, cv::Size(), 0.25, 0.25, interpolation_flag);
		break;

	case SUBSAMPLE_4X4_WITH_GAUSSIAN:
		cv::GaussianBlur(_src, _src, cv::Size(3, 3), 0.8, 0.8, cv::BORDER_DEFAULT);
		cv::resize(_src, _dst, cv::Size(), 0.25, 0.25, interpolation_flag);
		break;

	deafult:
		_src.copyTo(_dst);
	}

	return true;
}