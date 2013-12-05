

#ifndef _Klib_Mutex_H
#define _Klib_Mutex_H

#ifdef _MSC_VER
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace klib{
namespace kthread  {

    
///< win_mutex对象
class  win_mutex
{
public:
    win_mutex();
    ~win_mutex();

    ///< 判断是否存在该名字的mutex对象
    static BOOL is_exist_mutex(LPCTSTR lpName);

    ///< 创建mutex对象
    BOOL create_mutex(BOOL bInitialOwner, LPCTSTR lpName);

    ///< 打开metux对象
    BOOL open_mutex(LPCTSTR lpName);

    ///< 加锁
    void lock();

    ///< 解锁
    void unlock();

private:
    win_mutex(const win_mutex&other);
    win_mutex& operator=( const win_mutex& );

    HANDLE  m_hMutex;
};


///< 加锁mutex保护
class win_mutex_guard
{
public:
    win_mutex_guard(win_mutex& mutex) : m_mutex(mutex)
    {
        m_mutex.lock();
    }

    ~win_mutex_guard()
    {
        m_mutex.unlock();
    }

protected:
    win_mutex&  m_mutex;
};


}}

#endif // MUTEX_H__
