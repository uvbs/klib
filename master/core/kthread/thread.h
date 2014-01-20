
#pragma once

#include <functional>
#include <process.h>
#include <assert.h>

#include "event.h"
#include "../core/klib_core.h"
#include "../inttype.h"

using namespace klib::core;

namespace klib{
namespace kthread{


class Thread
{
public:
    typedef std::function<void(void*)> thread_func_type;

    enum TState
    {
        TS_INVALID = -1,
        TS_ACTIVE,
        TS_SUSPEND,
        TS_EXIT
    };

    struct param
    {
        Thread *    this_;
        void *      other_;
    };

public:
    Thread() : handle_(0), state_(TS_INVALID)
    {
        param_.other_ = 0;
        param_.this_  = 0;
    }
    ~Thread()
    {
        state_ = TS_EXIT;
    }

    bool start(const thread_func_type &thread_func, void *p = NULL )
    {
        assert( state_ == TS_INVALID && "thread::start : invalid call!" );
        param_.this_ = this;
        param_.other_ = p;
        func_ = thread_func;
        auto handle = (HANDLE) ::_beginthreadex( NULL, 0, ThreadFunc, &param_, 0, (unsigned int*)&id_ );
        handle_.Attach(handle);
        state_ = TS_ACTIVE;
        assert(handle != NULL);
        return handle_ != NULL;
    }

    void terminate()
    {
        assert( handle_ != NULL && "thread::terminate : the thread is not exist!" );
        ::TerminateThread( handle_, -1 ); 
        state_ = TS_EXIT;
    }

    void exit( bool should_wait = true )
    {
        assert( handle_ != NULL && "thread::exit : the thread is invalid!" );

        state_ = TS_EXIT;
        event_.signal();
    }

    DWORD wait( DWORD millseconds = INFINITE )
    {
        assert( handle_ != NULL && "thread::wait : the thread is invalid!" );
        return ::WaitForSingleObject(handle_, millseconds);
    }

    TState state()
    {
        return state_;
    }

private:
    static unsigned int __stdcall ThreadFunc( void *p )
    {
        param *_param = (param*) p;
        assert( _param != NULL && "ThreadFunc : invalid thread parameter!" );
        Thread *_thread = _param->this_ ;
        assert( _thread != NULL && "ThreadFunc : invalid thread parameter!" );

        assert( _thread->func_ && "thread : no thread function has been set!" );

        _thread->func_( p );

        _thread->state_ = TS_INVALID;
        _endthreadex( 0 );
        return 0;
    }

protected:
    thread_func_type func_;
    AutoFinalizerHandle<HANDLE> handle_;

    uint32_t id_;
    Event   event_;
    TState  state_;
    param   param_;
};



}}
