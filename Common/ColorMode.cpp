#include "ColorMode.h"

void Color2Grayscale(cv::Mat _src, cv::Mat& _dst)
{
	/* ----------------------------------------------------------
	 * �÷� �̹����� �׷��̽����Ϸ� ��ȯ
	 ---------------------------------------------------------- */
	if (_src.empty() || _src.channels() == 1)
	{
		_dst = _src.clone();
		return;
	}

	cv::cvtColor(_src, _dst, cv::COLOR_BGR2GRAY);
}