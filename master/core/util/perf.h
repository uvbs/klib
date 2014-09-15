#ifndef _perf_h_
#define _perf_h_

#ifdef _WIN32
#include <windows.h>
#include <stdint.h>
#include <functional>

class perf
{
public:
    perf();

    void start();
    void stop();
    double elapse_seconds();
    double elapse_microsecond();
    double elapse_counter();

protected:
    LARGE_INTEGER  begin_;
    LARGE_INTEGER  end_;
    
    LARGE_INTEGER frequence_;
};



#endif


class auto_perf
{
public:
    auto_perf(const char* str = nullptr);
    ~auto_perf();

protected:
    perf   pf_;
    const char* str_pref;
};

typedef std::function<void()> func_type;
class auto_func_perf
{
public:
    auto_func_perf(const func_type& func, const char* str = nullptr);
    ~auto_func_perf();

protected:
    perf   pf_;
    const char* str_pref;
    func_type func_;
};


#define SCOPEGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line)
#define AUTO_PERF(FUNCTION)     \
{           \
    auto_func_perf  SCOPEGUARD_LINENAME(EXIT, __LINE__) (([&](){ FUNCTION; }), #FUNCTION);              \
}



#endif  // _perf_h_