#ifndef _klib_stop_watch_h_
#define _klib_stop_watch_h_

#include <windows.h>
#include <time.h>

namespace klib{
namespace util{

class stop_watch
{
public:
    stop_watch(UINT uElapse)
    {
        set_elapse(uElapse);
        reset();
    }

    void set_elapse(UINT uElapse)
    {
        time_elapse_ = uElapse;
    }

    void reset()
    {
        start_time_ = _time64(NULL);
    }

    BOOL is_triggerd() const
    {
        UINT64 tm_now = _time64(NULL);
        if (tm_now >= (start_time_ + time_elapse_)) 
        {
            return TRUE;
        }
        return FALSE;
    }

protected:
    UINT64  start_time_;
    UINT    time_elapse_;


};



}}



#endif