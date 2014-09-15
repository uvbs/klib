#include "act_list.h"

#include "../core/scope_guard.h"
using namespace klib::core;

using namespace klib::pattern;

bool act_list::async_act(const act_func_type& func)
{
    klib::kthread::guard guard_(mutex_);
    act_lst_.push_back(func);
    return true;
}

bool act_list::async_act(act_func_type&& func)
{
    klib::kthread::guard guard_(mutex_);
    act_lst_.push_back(std::move(func));
    return true;
}

bool act_list::sync_act(const act_func_type& func)
{
    HANDLE done = CreateEvent(NULL, TRUE, FALSE, NULL);

    std::exception_ptr  pCxxException = nullptr;
    unsigned int        win32ExceptionCode = 0;
    EXCEPTION_POINTERS* win32ExceptionPointers = nullptr;

    std::function<void()> synchronousAction = [&] 
    { 
        defer( SetEvent(done); );

        try
        {
            func();
        }
        catch (std::exception_ptr e)
        {
            pCxxException = e;
        }
        catch (...)
        {
        }
    };

    bool r = this->async_act(synchronousAction);
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

bool act_list::exec()
{
    if (act_lst_.empty()) {
        return false;
    }

    act_func_type func;
    klib::kthread::lock_exec(mutex_, [&]()
        {
            if (!act_lst_.empty()) 
            {
                func = std::move(act_lst_.front());
                act_lst_.pop_front();
            }
        }
    );

    if (func)
        func();

    return true;
}