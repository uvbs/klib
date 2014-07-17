#ifndef _klib_perf_counter_h
#define _klib_perf_counter_h

#include <windows.h>

namespace klib {
namespace core {


// Performance Counter
class perf_counter
{
public:
    perf_counter(void)
    {
#ifdef _WIN32
        QueryPerformanceFrequency(&frequency_);
        QueryPerformanceCounter(&start_);
#else
        gettimeofday(&tm_, NULL);
#endif
    }

    ~perf_counter(void)
    {
    }

    void reset()
    {
#ifdef _WIN32
        QueryPerformanceCounter(&start_);
#else
        gettimeofday(&tm_, NULL);
#endif
    }

    LONGLONG delta()
    {
#ifdef _WIN32
        LARGE_INTEGER nEnd;
        QueryPerformanceCounter(&nEnd);
        return nEnd.QuadPart - start_.QuadPart;
#else
        struct timeval now;
        gettimeofday(&now, NULL);
        long cost = (now.tv_sec - tm_.tv_sec)*1000000 + (now.tv_usec - tm_.tv_usec);
        return cost;
#endif
    }

    float delta_seconds()
    {
#ifdef _WIN32
        LARGE_INTEGER nEnd;
        QueryPerformanceCounter(&nEnd);
        return (float)(nEnd.QuadPart - start_.QuadPart) / frequency_.QuadPart;
#else
        struct timeval now;
        gettimeofday(&now, NULL);
        long cost = (now.tv_sec - tm_.tv_sec)*1000000 + (now.tv_usec - tm_.tv_usec);
        cost /= 1000;
        return cost;
#endif
    }

protected:

#ifdef _WIN32
    LARGE_INTEGER start_;
    LARGE_INTEGER frequency_;
#else
    struct timeval tm_;
#endif
};



}}


#endif
