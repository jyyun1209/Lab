#include "File.h"

void Convert32FTo16U(const cv::Mat& _depth32, cv::Mat& _depth16, const float& _depth_scale, const float& _depth_offset)
{
	/*********************************************************************************
	* 32F Ÿ���� OpenCV Mat �����͸� �޾Ƽ� 16U Ÿ������ ��ȯ
	**********************************************************************************/

	float value32;

	for (int j = 0; j < _depth32.rows; j++)
	{
		for (int i = 0; i < _depth32.cols; i++)
		{
			value32 = _depth32.at<float>(j, i);	// mm
			if (!isfinite(value32))
				value32 = 0;
			else
				value32 = (value32 + _depth_offset) * _depth_scale;	// float to ushort
			_depth16.at<ushort>(j, i) = (ushort)value32;
		}
	}
}

void LoadImageToCV(cv::Mat& _image, std::string _path)
{
	_image = cv::imread(_path, cv::IMREAD_UNCHANGED);

	if (_image.empty())
	{
		throw std::invalid_argument("Empty Image");
	}
}

void SaveImage(cv::Mat image, std::string saveLocation, std::string _ext)
{
	/*********************************************************************************
	* OpenCV Mat Ÿ�� �����͸� �޾Ƽ� Image ���Ϸ� ����
	*
	* 8U, 16U, 32F Ÿ���� �̹����� ���� ����
	* 32F Ÿ���� �̹����� 16U Ÿ������ ��ȯ �� ���� (��ȯ �� �����Ϳ� ���� scale �ʿ�)
	**********************************************************************************/

	if (image.empty())
	{
		throw std::invalid_argument("Empty Image");
	}

	if (_ext != ".png" && _ext != ".jpg" && _ext != ".jpeg" && _ext != ".tif" && _ext != ".tiff")
	{
		throw std::invalid_argument("Invalid Image Extension (Only .png, .jpg, .jpeg .tif .tiff are supported)");
	}

	std::time_t now = std::time(nullptr);
	std::tm* t = std::localtime(&now);
	std::ostringstream oss;
	oss << std::put_time(t, "%Y_%m_%d_%H%M%S");
	std::string curr_time = oss.str();

	if (image.type() == CV_8UC1 || image.type() == CV_8UC3 || image.type() == CV_16UC1)
	{
		cv::imwrite(saveLocation + "\\" + curr_time + _ext, image);
	}
	else if (image.type() == CV_32FC1)
	{
		if (_ext == ".tif" || _ext == ".tiff")
		{
			cv::imwrite(saveLocation + "\\" + curr_time + _ext, image);
		}
		else
		{
			cv::Mat image16U = cv::Mat(image.size(), CV_16U);
			Convert32FTo16U(image, image16U);
			cv::imwrite(saveLocation + "\\" + curr_time + _ext, image16U);
		}
	}
	else
	{
		throw std::invalid_argument("Invalid Image Type (Only CV_8U, CV_16U, CV_32F types are supported)");
	}
}