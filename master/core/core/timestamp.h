#ifndef _klib_timestamp_h_
#define _klib_timestamp_h_

#include <time.h>
#include "../inttype.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/time.h>
#endif


namespace klib{
namespace core{

class Timestamp
{
public:
    Timestamp()
        : tms_stamp_(0)
    {}
    Timestamp(int64_t usTimestamp)
        : tms_stamp_(usTimestamp)
    {}
    Timestamp(const Timestamp &ts)
        : tms_stamp_(ts.tms_stamp_)
    {}

    string to_string()
    {
        char buf[32] = {0};
        int64_t seconds = tms_stamp_ / kMicroSecondsPerSecond;
        int64_t microseconds = tms_stamp_ % kMicroSecondsPerSecond;
        _snprintf(buf, sizeof(buf), "%lld.%06lld", seconds, microseconds);
        return buf;
    }

    string to_format_string()
    {
        char buf[32] = {0};
        time_t seconds = static_cast<time_t>(tms_stamp_ / kMicroSecondsPerSecond);
        int microseconds = static_cast<int>(tms_stamp_ % kMicroSecondsPerSecond);
        tm *tm_time = localtime(&seconds);

        _snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
            tm_time->tm_year + 1900, tm_time->tm_mon + 1, tm_time->tm_mday,
            tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec,
            microseconds);
        return buf;
    }

    time_t to_unix_timestamp() const 
	{ 
        return static_cast<time_t>(tms_stamp_ / kMicroSecondsPerSecond); 
    }
	
    int64_t timestamp() const { return tms_stamp_; }

    bool valid() const { return tms_stamp_ > 0; }

#ifdef WIN32
    LARGE_INTEGER to_large_int() const 
    {
        LARGE_INTEGER li;
        li.QuadPart = (tms_stamp_ * 10) + 116444736000000000;
        return li;
    }
#endif

    static Timestamp now()
    {
#ifdef WIN32
        FILETIME ft;
        ULARGE_INTEGER ui;  
        GetSystemTimeAsFileTime(&ft); // 精确到100ns
        ui.LowPart = ft.dwLowDateTime;  
        ui.HighPart = ft.dwHighDateTime;
        return Timestamp(static_cast<int64_t>(ui.QuadPart - 116444736000000000) / 10);
#else
        struct timeval tv;
        gettimeofday(&tv, NULL);
        int64_t seconds = tv.tv_sec;
        return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
#endif
    }

    static Timestamp invalid() { return Timestamp(); }

    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t tms_stamp_;
};

inline bool operator<(Timestamp lhs, Timestamp rhs)
{
    return lhs.timestamp() < rhs.timestamp();
}

inline bool operator<=(Timestamp lhs, Timestamp rhs)
{
    return lhs.timestamp() <= rhs.timestamp();
}

inline bool operator==(Timestamp lhs, Timestamp rhs)
{
    return lhs.timestamp() == rhs.timestamp();
}

}}

#endif // _klib_timestamp_h_
