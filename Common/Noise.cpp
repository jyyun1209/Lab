#include "Noise.h"

void SaltAndPepper(cv::Mat _src, cv::Mat& _dst, int _strength)
{
	/* ----------------------------------------------------------
	 * 소금과 후추 노이즈
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

void SpeckleNoise(cv::Mat _src, cv::Mat& _dst, float _stddev)
{
	if (_src.empty() || _stddev == 0)
	{
		_dst = _src.clone();
		return;
	}

	_src.convertTo(_dst, CV_32F);
	_dst = _dst / 255.0f;

	cv::Mat noise(_dst.size(), _dst.type());
	std::default_random_engine rng(std::random_device{}());
	std::normal_distribution<float> dist(0.0f, _stddev);

	for (int r = 0; r < _dst.rows; r++)
	{
		float* img_row = _dst.ptr<float>(r);
		float* noise_row = noise.ptr<float>(r);
		for (int c = 0; c < _dst.cols * _dst.channels(); c++)
		{
			noise_row[c] = dist(rng);
		}
	}
	
	noise = noise + 1;
	_dst = _dst.mul(noise);
	_dst = _dst * 255.0f;
	_dst.convertTo(_dst, _src.type());
}