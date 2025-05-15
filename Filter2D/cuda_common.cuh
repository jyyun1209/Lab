#pragma once

#define CUDA_CHECK(val) {\
	if (val != cudaSuccess) {\
		fprintf(stderr, "Error %s at line %d in file %s\n", cudaGetErrorString(val), __LINE__, __FILE__);\
		exit(val);\
	}\
}