#include "Noise.h"

void SaltAndPepper(cv::Mat _src, cv::Mat& _dst, int _strength)
{
	/* ----------------------------------------------------------
	 * 소금과 후추 노이즈 추가
	 * _strength: 노이즈 강도 (0-100)
	 ---------------------------------------------------------- */
	if (_src.empty())
	{
		_dst = _src.clone();
		return;
	}

	cv::Mat noise = cv::Mat::zeros(_src.size(), CV_8UC1);
	cv::randu(noise, 0, 100);

	_dst = _src.clone();
	for (int i = 0; i < noise.rows; i++)
	{
		for (int j = 0; j < noise.cols; j++)
		{
			if (noise.at<uchar>(i, j) < _strength)
			{
				if (rand() % 2 == 0) // Salt
				{
					if (_src.channels() == 1)
					{
						_dst.at<uchar>(i, j) = 255;
					}
					else
					{
						_dst.at<cv::Vec3b>(i, j) = cv::Vec3b(255, 255, 255);
					}
				}
				else // Pepper
				{
					if (_src.channels() == 1)
					{
						_dst.at<uchar>(i, j) = 0;
					}
					else
					{
						_dst.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0); // 후추
					}
				}
			}
		}
	}
}