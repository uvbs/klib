#include "perf.h"
#include "stdio.h"

perf::perf()
{
    QueryPerformanceFrequency(&frequence_);
}

void perf::start()
{
    QueryPerformanceCounter(&begin_);
}

void perf::stop()
{
    QueryPerformanceCounter(&end_);
}

double perf::elapse_seconds()
{
    double t = (end_.QuadPart - begin_.QuadPart)
        / frequence_.QuadPart;

    return t;
}

double perf::elapse_microsecond()
{
    double t = (end_.QuadPart - begin_.QuadPart) * 1000
        / frequence_.QuadPart;

    return  t;
}

double perf::elapse_counter()
{
    double t = (end_.QuadPart - begin_.QuadPart);

    return t;
}


////////////////////////////////////////////////////////////////////////////////
//

auto_perf::auto_perf(const char* str /*= nullptr*/)
    : str_pref(str)
{
    pf_.start();
}

auto_perf::~auto_perf()
{
    pf_.stop();

    if (str_pref)
        printf(str_pref);

    printf("use time: %f \r\n\r\n", pf_.elapse_counter());
}

auto_func_perf::auto_func_perf(const func_type& func, const char* str /*= nullptr*/)
    : str_pref(str)
{
    pf_.start();
    func();
}

auto_func_perf::~auto_func_perf()
{
    pf_.stop();

    if (str_pref)
    {
        printf(str_pref);
        printf(" \r\n");
    }

    printf("use time: %f \r\n\r\n", pf_.elapse_counter());
}
