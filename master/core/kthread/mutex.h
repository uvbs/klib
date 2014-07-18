#ifndef _klib_mutex_h_
#define _klib_mutex_h_


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
typedef CRITICAL_SECTION pthread_mutex_t;
#else
#include <pthread.h>
#endif

#include "../inttype.h"


namespace klib {
namespace kthread {

class mutex
{
    mutex(const mutex&other);
    mutex operator = (const mutex& other);
public:
    mutex()
        : thread_id_(0)
    {
#ifdef WIN32
        InitializeCriticalSection(&mutex_);
#else
        pthread_mutex_init(&mutex_, NULL);
#endif
    }
    ~mutex()
    {
#ifdef WIN32
        DeleteCriticalSection(&mutex_);
#else
		pthread_mutex_destroy(&mutex_);
#endif
    }
    
    void lock()
    {
#ifdef WIN32
        EnterCriticalSection(&mutex_);
        thread_id_ = static_cast<uint32_t>(GetCurrentThreadId());
#else
        pthread_mutex_lock(&mutex_);
        thread_id_ = static_cast<uint32_t>(pthread_self());
#endif
    }
    
    void unlock()
    {
        thread_id_ = 0;
#ifdef WIN32
        LeaveCriticalSection(&mutex_);
#else
        pthread_mutex_unlock(&mutex_);
#endif
    }
    
	pthread_mutex_t *getMutex()
	{
		return &mutex_;
	}
	
private:

    uint32_t thread_id_;
    pthread_mutex_t mutex_;
};

class guard
{
public:
    guard(mutex &m)
        : mutex_(m)
    {
        mutex_.lock();
    }
    ~guard()
    {
        mutex_.unlock();
    }

private:
    mutex &mutex_;
};

#define MutexGuard(x) error "Missing guard object name"

}}

#endif // _klib_mutex_h_
