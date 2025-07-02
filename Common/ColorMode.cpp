#include "ColorMode.h"

void Color2Grayscale(cv::Mat _src, cv::Mat& _dst)
{
	/* ----------------------------------------------------------
	 * 컬러 이미지를 그레이스케일로 변환
	 ---------------------------------------------------------- */
	if (_src.empty() || _src.channels() == 1)
	{
		_dst = _src.clone();
		return;
	}

	cv::cvtColor(_src, _dst, cv::COLOR_BGR2GRAY);
}