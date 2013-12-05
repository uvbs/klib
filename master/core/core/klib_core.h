
#pragma once


#include <functional>
#include <windows.h>
#include "../macro.h"
#include "noncopyable.h"

namespace klib {
namespace core {

#define  KLIB_ASSERT(expr)        \
    _ASSERT(expr)

//----------------------------------------------------------------------
template<class T> inline void checked_delete(T * x)
{  
    // intentionally complex - simplification causes regressions
    typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
    (void) sizeof(type_must_be_complete);
    delete x;
}

template<class T> inline void checked_array_delete(T * x)
{
    typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
    (void) sizeof(type_must_be_complete);
    delete [] x;
}

//----------------------------------------------------------------------
class AutoFinalizer : public noncopyable
{
public:
    typedef std::function<void()> Func;

    AutoFinalizer(const Func& f) : f_(f) {}
    ~AutoFinalizer() { f_(); }
private:
    Func f_;
};

//----------------------------------------------------------------------
template<typename HandleType>
class AutoFinalizerHandle
{
public:
    AutoFinalizerHandle() : handle_(NULL)  {}
    AutoFinalizerHandle(HandleType handle) : handle_(handle) { }
    ~AutoFinalizerHandle() {    SAFE_CLOSE_HANDLE(handle_);    }

    void Attach(HandleType handle) {
        handle_ = handle;
    }
    operator HandleType() const { return handle_; }

protected:
    HandleType handle_;
};

template<>
class AutoFinalizerHandle<FILE*>
{
public:
    typedef FILE*  HandleType;

    AutoFinalizerHandle() : handle_(NULL)  {  }
    AutoFinalizerHandle(HandleType handle) : handle_(handle)   {  }
    ~AutoFinalizerHandle() {
        if (handle_) {
            fclose(handle_);
            handle_ = NULL;
        }
    }

    void Attach(HandleType handle) {
        handle_ = handle;
    }
    operator HandleType() const { return handle_; }

protected:
    HandleType handle_;
};


}}
