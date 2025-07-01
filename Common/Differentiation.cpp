#include "Differentiation.h"

void Diff_Partial_X(cv::Mat _src, cv::Mat& _dst, DIFF_PARTIAL_MODE _mode, DIFF_DIRECTION _dir)
{
	/* ----------------------------------------------------------
	 * X 방향 편미분
	---------------------------------------------------------- */
	OutputDebugString(L"Diff_Partial_X called\n");

	if (_src.empty())
	{
		OutputDebugString(L"Input image is empty.\n");
	}

	switch (_mode)
	{
		case DIFF_PARTIAL_OPENCV:
			Diff_Partial_X_OpenCV(_src, _dst, _dir);
			break;

		case DIFF_PARTIAL_CPP:
			Diff_Partial_X_CPP(_src, _dst, _dir);
			break;

		default:
			OutputDebugString(L"지원하지 않는 모드입니다.\n");
			_dst = _src.clone();
			break;
	}

}

void Diff_Partial_Y(cv::Mat _src, cv::Mat& _dst, DIFF_PARTIAL_MODE _mode, DIFF_DIRECTION _dir)
{
	/* ----------------------------------------------------------
	 * Y 방향 편미분
	---------------------------------------------------------- */
	OutputDebugString(L"Diff_Partial_Y called\n");

	if (_src.empty())
	{
		OutputDebugString(L"Input image is empty.\n");
	}

	switch (_mode)
	{
	case DIFF_PARTIAL_OPENCV:
		Diff_Partial_X_OpenCV(_src.t(), _dst, _dir);
		_dst = _dst.t();
		break;

	case DIFF_PARTIAL_CPP:
		Diff_Partial_X_CPP(_src.t(), _dst, _dir);
		_dst = _dst.t();
		break;

	default:
		OutputDebugString(L"지원하지 않는 모드입니다.\n");
		_dst = _src.clone();
		break;
	}
}

void Laplacian(cv::Mat _src, cv::Mat& _dst, LAPLACIAN_MODE _mode)
{
	/* ----------------------------------------------------------
	 * 라플라시안
	---------------------------------------------------------- */
	OutputDebugString(L"Laplacian called\n");

	if (_src.empty())
	{
		OutputDebugString(L"Input image is empty.\n");
	}

	switch (_mode)
	{
	case LAPLACIAN_OPENCV:
		Laplacian_OpenCV(_src, _dst);
		break;

	default:
		OutputDebugString(L"지원하지 않는 모드입니다.\n");
		_dst = _src.clone();
		break;
	}
}

void Diff_Partial_X_OpenCV(cv::Mat _src, cv::Mat& _dst, DIFF_DIRECTION _dir)
{
	OutputDebugString(L"Diff_Partial_X OpenCV Mode called\n");

	if (_src.empty())
	{
		OutputDebugString(L"Input image is empty.\n");
	}

	_src.convertTo(_dst, CV_32FC1);

	cv::Mat kernel;
	cv::Point anchor;
	switch (_dir)
	{
	case DIFF_DIRECTION_CENTER:
		// 원래 2h 이기 때문에 1/2 하는 것이 맞으나 근사
		anchor = cv::Point(1, 0);
		kernel = (cv::Mat_<float>(1, 3) << -1, 0, 1);
		break;

	case DIFF_DIRECTION_LEFT:
		anchor = cv::Point(0, 0);
		kernel = (cv::Mat_<float>(1, 2) << -1, 1);
		break;

	case DIFF_DIRECTION_RIGHT:
		anchor = cv::Point(1, 0);
		kernel = (cv::Mat_<float>(1, 2) << -1, 1);
		break;

	default:
		_dst = _src.clone();
		return;
	}

	if (_src.channels() == 1)
	{
		cv::filter2D(_dst, _dst, CV_32FC1, kernel, anchor, 0, cv::BORDER_REPLICATE);
		_dst.convertTo(_dst, CV_8UC1);
	}
	else
	{
		OutputDebugString(L"현재 1 Channel 이미지만 가능합니다.\n");
	}
}

void Diff_Partial_X_CPP(cv::Mat _src, cv::Mat& _dst, DIFF_DIRECTION _dir)
{
	// 현재 DIFF_DIRECTION 적용 안됨
	OutputDebugString(L"Diff_Partial_X C++ Mode called\n");

	if (_src.empty())
	{
		OutputDebugString(L"Input image is empty.\n");
	}

	_src.convertTo(_dst, CV_32FC1);

	if (_src.channels() == 1)
	{
		for (int r = 0; r < _src.rows; r++)
		{
			for (int c = _src.cols - 1; c > 0; c--)
			{
				_dst.at<float>(r, c) = _dst.at<float>(r, c) - _dst.at<float>(r, c - 1);
			}
			_dst.at<float>(r, 0) = 0; // 마지막 열은 편미분이 정의되지 않으므로 0으로 설정 (BORDER_REPLICATE와 같은 효과)
		}
		_dst.convertTo(_dst, CV_8UC1);
	}
	else
	{
		OutputDebugString(L"현재 1 Channel 이미지만 가능합니다.\n");
	}
}

void Laplacian_OpenCV(cv::Mat _src, cv::Mat& _dst)
{
	/*-----------------------------------------------------------
	* 커널:
	* 
	* 0	 1	0
	* 1	-4	1
	* 0	 1	0
	* 
	* => [1 -2  1]이 2차 편미분이기 때문
	* ---------------------------------------------------------*/
	OutputDebugString(L"Laplacian OpenCV Mode called\n");

	if (_src.empty())
	{
		OutputDebugString(L"Input image is empty.\n");
	}

	_src.convertTo(_dst, CV_32FC1);

	if (_src.channels() == 1)
	{
		cv::Laplacian(_dst, _dst, CV_32FC1, 1, 1, 0, cv::BORDER_REPLICATE);
		_dst.convertTo(_dst, _src.type());
	}
	else
	{
		OutputDebugString(L"현재 1 Channel 이미지만 가능합니다.\n");
	}
}