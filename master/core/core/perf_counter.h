#pragma once

#include <windows.h>

namespace klib {
namespace core {


// Performance Counter
class perf_counter
{
public:
	perf_counter(void)
	{
		QueryPerformanceFrequency(&m_nFrequency);
		QueryPerformanceCounter(&m_nStart);
	}

	~perf_counter(void)
	{
	}

	void reset()
	{
		QueryPerformanceCounter(&m_nStart);
	}

	LONGLONG delta()
	{
		LARGE_INTEGER nEnd;
		QueryPerformanceCounter(&nEnd);
		return nEnd.QuadPart - m_nStart.QuadPart;
	}

	float delta_seconds()
	{
		LARGE_INTEGER nEnd;
		QueryPerformanceCounter(&nEnd);
		return (float)(nEnd.QuadPart - m_nStart.QuadPart) / m_nFrequency.QuadPart;
	}

protected:
	LARGE_INTEGER m_nStart;
	LARGE_INTEGER m_nFrequency;
};



}}
