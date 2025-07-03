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

void RGBGain(cv::Mat _src, cv::Mat& _dst, float _rGain, float _gGain, float _bGain)
{
	if (_src.empty() || _src.channels() != 3)
	{
		_dst = _src.clone();
		return;
	}

	_dst = _src.clone();
	
	std::vector<cv::Mat> channels(3);
	cv::split(_dst, channels);

	channels[0] = channels[0] * _bGain; // Blue
	channels[1] = channels[1] * _gGain; // Green
	channels[2] = channels[2] * _rGain; // Red

	cv::merge(channels, _dst);
}