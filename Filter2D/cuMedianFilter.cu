#include "cuMedianFilter.cuh"

/* Insertion sort */
__device__ void d_insertionSort(float* arr, int size)
{
	float curr = 0;
	int j = 0;
	for (int i = 1; i < size; i++)
	{
		curr = arr[i];
		j = i - 1;

		while (j >= 0 && arr[j] > curr)
		{
			arr[j + 1] = arr[j];
			j = j - 1;
		}
		arr[j + 1] = curr;
	}
}

/* Median Filter */
__device__ int borderReplicate(int idx, int size)
{
	if (idx < 0)
	{
		return 0;
	}
	else if (idx >= size)
	{
		return size - 1;
	}
	else
	{
		return idx;
	}
}

__device__ float d_borderInterpolate(float* d_inputImage, int x, int y, int width, int height, int borderType)
{
	switch (borderType)
	{
	case CUDA_BORDER_REPLICATE:
		x = borderReplicate(x, width);
		y = borderReplicate(y, height);
	}
	return d_inputImage[x + y * width];
}

__global__ void d_medianBlur(float* d_inputImage, float* d_outputImage, int width, int height, int kernel_x, int kernel_y, int borderType, bool finiteOnly)
{
	int x = blockIdx.x * blockDim.x + threadIdx.x;	// Width
	int y = blockIdx.y * blockDim.y + threadIdx.y;	// Height

	if (x < width && y < height)
	{
		int kw05 = kernel_x >> 1;
		int kh05 = kernel_y >> 1;
		int cnt = 0;
		float kernel[256] = { 0 };	// CHK: Dynamic memory allocation 도입 필요
		float curr_val = 0;
		for (int j = -kh05; j <= kh05; j++)
		{
			for (int i = -kw05; i <= kw05; i++)
			{
				if (x - kw05 >= 0 && y - kh05 >= 0 && x + kw05 < width && y + kh05 < height)
				{
					curr_val = d_inputImage[(y + j) * width + (x + i)];
				}
				else
				{
					curr_val = d_borderInterpolate(d_inputImage, x + i, y + j, width, height, borderType); // CHK: Border Type 다양화 필요
				}

				if (finiteOnly)
				{
					if (isfinite(curr_val))
					{
						kernel[cnt] = curr_val;
						cnt++;
					}
				}
				else
				{
					kernel[cnt] = curr_val;
					cnt++;
				}
			}
		}

		int threshold = 0;
		if (finiteOnly)
		{
			threshold = (kernel_x * kernel_y) / 2;
		}

		if (cnt > threshold)
		{
			d_insertionSort(kernel, cnt);
			d_outputImage[width * y + x] = kernel[cnt >> 1];
		}
		else
		{
			d_outputImage[width * y + x] = d_inputImage[width * y + x];
		}
	}
}

void cuMedianBlur(cv::Mat h_inputImage, cv::Mat& h_outputImage, int kernel_x, int kernel_y, int borderType, bool finiteOnly)
{
	//TIMER_CUDA();
	if (h_inputImage.empty() || h_inputImage.channels() != 1)
	{
		h_outputImage = h_inputImage.clone();
		return;
	}

	int src_type = h_inputImage.type();
	h_inputImage.convertTo(h_inputImage, CV_32F);

	float* d_inputImage, * d_outputImage;
	int size = h_inputImage.rows * h_inputImage.cols * sizeof(float);

	/* Memory Allocation */
	CUDA_CHECK(cudaMalloc(&d_inputImage, size));
	CUDA_CHECK(cudaMalloc(&d_outputImage, size));

	cudaStream_t stream;
	CUDA_CHECK(cudaStreamCreate(&stream));

	/* Memory Copy (Host to Device) */
	CUDA_CHECK(cudaMemcpyAsync(d_inputImage, (float*)h_inputImage.data, size, cudaMemcpyHostToDevice, stream));

	/* Run Kernel Function */
	dim3 dimBlock(8, 8);	// (width, height) (블록 당 최대 쓰레드 수: 1024)
	dim3 dimGrid(ceil(h_inputImage.cols / (double)dimBlock.x), ceil(h_inputImage.rows / (double)dimBlock.y));	// (width, height)
	d_medianBlur <<< dimGrid, dimBlock, 0, stream >>> (d_inputImage, d_outputImage, h_inputImage.cols, h_inputImage.rows, kernel_x, kernel_y, borderType, finiteOnly);

	/* Memory Copy (Device to Host) */
	CUDA_CHECK(cudaMemcpyAsync((float*)h_outputImage.data, d_outputImage, size, cudaMemcpyDeviceToHost, stream));

	h_outputImage.convertTo(h_outputImage, src_type);

	/* Memory Deallocation */
	CUDA_CHECK(cudaStreamDestroy(stream));
	CUDA_CHECK(cudaFree(d_inputImage));
	CUDA_CHECK(cudaFree(d_outputImage));
}