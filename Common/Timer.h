#pragma once

#include <iostream>
#include <chrono>
#include <Windows.h>
//#include <nvtx3/nvToolsExt.h>

#define TIMER() Timer timer##__LINE__(__FUNCTION__) // timer##__LINE__ : timer + __LINE__ 으로, 해당 코드가 있는 라인의 번호로 timer 이름을 생성
//#define TIMER_CUDA() Timer_CUDA cudaTimer##__LINE__(__FUNCTION__)

class Timer {
public:
	Timer(const std::string& name) : name(name), start(std::chrono::high_resolution_clock::now()) {}
	~Timer()
	{
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = end - start;
		std::cout << "Function [" << name << "] : " << elapsed.count() << " ms" << std::endl;
		OutputDebugStringA(("Function [" + name + "] : " + std::to_string(elapsed.count()) + " ms\n").c_str());
	}

private:
	std::string name;
	std::chrono::high_resolution_clock::time_point start;
};

//class Timer_CUDA {
//public:
//	Timer_CUDA(const std::string& name)
//	{
//		nvtxNameOsThread(GetCurrentThreadId(), name.data());
//		nvtxRangePush(name.data());
//	}
//	~Timer_CUDA()
//	{
//		nvtxRangePop();
//	}
//private:
//	std::string name;
//};