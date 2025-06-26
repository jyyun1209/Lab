#include "Differentiation.h"

void Diff_Partial_X(cv::Mat _src, cv::Mat& _dst)
{
	/* ----------------------------------------------------------
	 * X ���� ��̺�
	---------------------------------------------------------- */
	OutputDebugString(L"Diff_Partial_X called\n");

	if (_src.empty())
	{
		OutputDebugString(L"Input image is empty.\n");
	}

	_src.convertTo(_dst, CV_32FC1);

	if (_src.channels() == 1)
	{
		cv::Mat kernel = (cv::Mat_<float>(1, 2) << -1, 1);
		cv::filter2D(_dst, _dst, CV_32FC1, kernel);
		_dst.convertTo(_dst, CV_8UC1);
	}
	else
	{
		OutputDebugString(L"���� 1 Channel �̹����� �����մϴ�.\n");
	}
}

void Diff_Partial_Y(cv::Mat _src, cv::Mat& _dst)
{
	/* ----------------------------------------------------------
	 * Y ���� ��̺�
	---------------------------------------------------------- */
	OutputDebugString(L"Diff_Partial_Y called\n");

	if (_src.empty())
	{
		OutputDebugString(L"Input image is empty.\n");
	}

	_src.convertTo(_dst, CV_32FC1);

	if (_src.channels() == 1)
	{
		cv::Mat kernel = (cv::Mat_<float>(1, 2) << -1, 1);
		cv::filter2D(_dst, _dst, CV_32FC1, kernel.t());
		_dst.convertTo(_dst, CV_8UC1);
	}
	else
	{
		OutputDebugString(L"���� 1 Channel �̹����� �����մϴ�.\n");
	}
}