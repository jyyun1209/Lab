#include "Differentiation.h"

void Diff_Partial_X(cv::Mat _src, cv::Mat& _dst, DIFF_PARTIAL_MODE _mode)
{
	/* ----------------------------------------------------------
	 * X ���� ��̺�
	---------------------------------------------------------- */
	OutputDebugString(L"Diff_Partial_X called\n");

	if (_src.empty())
	{
		OutputDebugString(L"Input image is empty.\n");
	}

	switch (_mode)
	{
		case DIFF_PARTIAL_OPENCV:
			Diff_Partial_X_OpenCV(_src, _dst);
			break;

		case DIFF_PARTIAL_CPP:
			Diff_Partial_X_CPP(_src, _dst);
			break;

		default:
			OutputDebugString(L"�������� �ʴ� ����Դϴ�.\n");
			_dst = _src.clone();
			break;
	}

}

void Diff_Partial_Y(cv::Mat _src, cv::Mat& _dst, DIFF_PARTIAL_MODE _mode)
{
	/* ----------------------------------------------------------
	 * Y ���� ��̺�
	---------------------------------------------------------- */
	OutputDebugString(L"Diff_Partial_Y called\n");

	if (_src.empty())
	{
		OutputDebugString(L"Input image is empty.\n");
	}

	switch (_mode)
	{
	case DIFF_PARTIAL_OPENCV:
		Diff_Partial_X_OpenCV(_src.t(), _dst);
		_dst = _dst.t();
		break;

	case DIFF_PARTIAL_CPP:
		Diff_Partial_X_CPP(_src.t(), _dst);
		_dst = _dst.t();
		break;

	default:
		OutputDebugString(L"�������� �ʴ� ����Դϴ�.\n");
		_dst = _src.clone();
		break;
	}
}

void Laplacian(cv::Mat _src, cv::Mat& _dst, LAPLACIAN_MODE _mode)
{
	/* ----------------------------------------------------------
	 * ���ö�þ�
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

	case LAPLACIAN_CPP:
		Laplacian_CPP(_src, _dst);
		break;

	default:
		OutputDebugString(L"�������� �ʴ� ����Դϴ�.\n");
		_dst = _src.clone();
		break;
	}
}

void Diff_Partial_X_OpenCV(cv::Mat _src, cv::Mat& _dst)
{
	OutputDebugString(L"Diff_Partial_X OpenCV Mode called\n");

	if (_src.empty())
	{
		OutputDebugString(L"Input image is empty.\n");
	}

	_src.convertTo(_dst, CV_32FC1);

	if (_src.channels() == 1)
	{
		cv::Mat kernel = (cv::Mat_<float>(1, 2) << -1, 1);
		cv::filter2D(_dst, _dst, CV_32FC1, kernel, cv::Point(-1, -1), 0, cv::BORDER_REPLICATE);
		_dst.convertTo(_dst, CV_8UC1);
	}
	else
	{
		OutputDebugString(L"���� 1 Channel �̹����� �����մϴ�.\n");
	}
}

void Diff_Partial_X_CPP(cv::Mat _src, cv::Mat& _dst)
{
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
			_dst.at<float>(r, 0) = 0; // ������ ���� ��̺��� ���ǵ��� �����Ƿ� 0���� ���� (BORDER_REPLICATE�� ���� ȿ��)
		}
		_dst.convertTo(_dst, CV_8UC1);
	}
	else
	{
		OutputDebugString(L"���� 1 Channel �̹����� �����մϴ�.\n");
	}
}

void Laplacian_OpenCV(cv::Mat _src, cv::Mat& _dst)
{
	OutputDebugString(L"Laplacian OpenCV Mode called\n");

	if (_src.empty())
	{
		OutputDebugString(L"Input image is empty.\n");
	}

	_src.convertTo(_dst, CV_32FC1);

	if (_src.channels() == 1)
	{
		cv::Laplacian(_dst, _dst, CV_32FC1, 1, 1, 0, cv::BORDER_REPLICATE);
		_dst.convertTo(_dst, CV_8UC1);
	}
	else
	{
		OutputDebugString(L"���� 1 Channel �̹����� �����մϴ�.\n");
	}
}

void Laplacian_CPP(cv::Mat _src, cv::Mat& _dst)
{
	OutputDebugString(L"Laplacian C++ Mode called\n");

	if (_src.empty())
	{
		OutputDebugString(L"Input image is empty.\n");
	}

	cv::Mat xx, yy;
	_src.convertTo(xx, CV_32FC1);
	_src.convertTo(yy, CV_32FC1);

	if (_src.channels() == 1)
	{
		Diff_Partial_X(xx, xx, DIFF_PARTIAL_CPP);
		Diff_Partial_X(xx, xx, DIFF_PARTIAL_CPP);
		Diff_Partial_Y(yy, yy, DIFF_PARTIAL_CPP);
		Diff_Partial_Y(yy, yy, DIFF_PARTIAL_CPP);

		_dst = xx + yy;

		_dst.convertTo(_dst, CV_8UC1);
	}
	else
	{
		OutputDebugString(L"���� 1 Channel �̹����� �����մϴ�.\n");
	}
}