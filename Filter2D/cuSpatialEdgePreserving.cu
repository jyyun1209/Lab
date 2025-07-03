#include "cuSpatialEdgePreserving.cuh"

void cuSpatialEdgePreserving(cv::Mat h_inputImage, cv::Mat& h_outputImage, int _sigmaS, double _sigmaR, int _numIter)
{
	float *d_inputImage, *d_outputImage;

}