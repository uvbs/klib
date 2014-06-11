#ifndef _klib_sync_invoke_h
#define _klib_sync_invoke_h

#include "../core/scope_guard.h"
#include <functional>

namespace klib{
namespace pattern{

using namespace klib::core;


#define UM_CALL   (WM_USER + 300)



class sync_invoke
{
public:
    sync_invoke(void);
    ~sync_invoke(void);

    bool start();

    bool stop();

    bool send(std::function<void()> action) 
    {
        HANDLE done = CreateEvent(NULL, TRUE, FALSE, NULL);

        std::exception_ptr  pCxxException = nullptr;
        unsigned int        win32ExceptionCode = 0;
        EXCEPTION_POINTERS* win32ExceptionPointers = nullptr;

        std::function<void()> synchronousAction = [&] 
        { 
            ON_SCOPE_EXIT( SetEvent(done); );
            
            try
            {
                action();
            }
            catch (std::exception_ptr e)
            {
                pCxxException = e;
            }
            catch (...)
            {
            }
        };

        bool r = post(synchronousAction);

        if(r)
        {
            WaitForSingleObject(done, INFINITE);
            CloseHandle(done);

            // propagate error (if any) to the calling thread
            if(!(pCxxException == nullptr))
            {
                std::rethrow_exception(pCxxException);
            }
        }
        return r;
    }

protected:
    bool post(std::function<void()>& fun)
    {
        BOOL bPostResult = FALSE;
        bPostResult = PostThreadMessage(m_dwThreadID, UM_CALL, (WPARAM)&fun, 0);
        while (!bPostResult) {
            bPostResult = PostThreadMessage(m_dwThreadID, UM_CALL, (WPARAM)&fun, 0);
            Sleep(100);
        }
        
        return true;
    }

protected:
    static unsigned int WINAPI workthread(void* param);

    HANDLE m_hThread;
    DWORD  m_dwThreadID;
};



}}



#endif
