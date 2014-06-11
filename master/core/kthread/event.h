#ifndef _klib_event_h
#define _klib_event_h

#include "../macro.h"


#ifdef _MSC_VER
#include <windows.h>
#include <tchar.h>
#endif


namespace klib {
namespace kthread {

#ifdef _MSC_VER

// 事件对象
class win_event
{
public:
    win_event() : handle_(NULL) {
        handle_ = CreateEvent(NULL, FALSE, FALSE, NULL);
    }

    win_event(TCHAR* name, BOOL bManualReset = FALSE)
    {
        handle_ = CreateEvent(NULL, bManualReset, FALSE, name);
    }
    ~win_event()
    {
        close();
    }

    void signal()
    {
        SetEvent(handle_);
    }

    void reset()
    {
        ResetEvent(handle_);
    }

    DWORD wait(DWORD dwTime = INFINITE)
    {
        return WaitForSingleObject(handle_, dwTime);
    }

    void close()
    {
        SAFE_CLOSE_HANDLE(handle_);
    }

protected:
    HANDLE  handle_;
};


typedef win_event Event;

#endif


}}


#endif
