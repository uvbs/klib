
#ifndef __Klib_READWRITESECTION_H__
#define __Klib_READWRITESECTION_H__

#include "auto_lock.h"
#include <windows.h>

namespace klib {
namespace kthread {


class CReadWriteSection
{
public:
    CReadWriteSection()
    {
        m_lReaders = -1;
    }

    ~CReadWriteSection()
    {
    }

    void enter_read()
    {
        m_csEnterRead.lock();
        if(!InterlockedIncrement(&m_lReaders))
        {
            m_csWriter.lock();
        }
    }
    void leave_read()
    {
        if(InterlockedDecrement(&m_lReaders) < 0)
        {
            m_csWriter.unlock();
        }
    }

    void enter_write()
    {
        m_csEnterRead.lock();
        m_csWriter.lock();
    }

    void leave_write()
    {
        m_csWriter.unlock();
        m_csEnterRead.unlock();
    }

    // Note that readers will continue reading underneath this critical section
    auto_cs* get_lock()
    {
        return & m_csEnterRead;
    }

private:
    auto_cs             m_csEnterRead;
    auto_cs             m_csWriter;
    LONG                m_lReaders;
};

class  CReadLock
{
public:
    CReadLock(CReadWriteSection& crws) { (rws_ = &crws)->enter_read(); }
    ~CReadLock() { rws_->leave_read(); }
private:
    CReadWriteSection* rws_;
};

class  CWriteLock
{
public:
    CWriteLock(CReadWriteSection& crws) { (rws_=&crws)->enter_write(); }
    ~CWriteLock() { rws_->leave_write(); }
private:
    CReadWriteSection* rws_;
};

#define LOCK_READ(crws)     CReadLock  crws##_readlock(crws)
#define LOCK_WRITE(crws)    CWriteLock crws##_writelock(crws)


}}



#endif //__XINDOWSUTIL_MT_READWRITESECTION_H__