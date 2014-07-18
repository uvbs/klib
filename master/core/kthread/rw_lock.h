#ifndef _RW_LOCK_h_
#define _RW_LOCK_h_

#include "mutex.h"

namespace klib {
namespace kthread {


#define LOCK_LEVEL_NONE 0
#define LOCK_LEVEL_READ 1           ///< ¶ÁËø
#define LOCK_LEVEL_WRITE 2          ///< Ð´Ëø


///< ¶ÁÐ´Ëø
class read_write_lock
{
protected:
    int    m_currentLevel;
    int    m_readCount;  
    HANDLE m_unlockEvent;
    HANDLE m_accessMutex;

    mutex state_change_mutex_;
    
public:
    read_write_lock()
    {
        m_currentLevel = LOCK_LEVEL_NONE;
        m_readCount    = 0;
        m_unlockEvent  = ::CreateEvent( NULL, TRUE, FALSE, NULL );
        m_accessMutex  = ::CreateMutex( NULL, FALSE, NULL );
    }
    ~read_write_lock()
    {
        if ( m_accessMutex ) ::CloseHandle( m_accessMutex );
        if ( m_unlockEvent ) ::CloseHandle( m_unlockEvent );
    }
    
    bool lock( int level, int timeout = INFINITE)
    {
        bool  bresult    = true;
        DWORD waitResult = 0;
        
        waitResult = ::WaitForSingleObject(m_accessMutex, timeout);
        if ( waitResult != WAIT_OBJECT_0 )  return false;
        
        if ( level == LOCK_LEVEL_READ && m_currentLevel != LOCK_LEVEL_WRITE )
        {
            guard gd(&state_change_mutex_);

            m_currentLevel = level;
            m_readCount += 1;
            ::ResetEvent( m_unlockEvent );
        }
        else if ( level == LOCK_LEVEL_READ && m_currentLevel == LOCK_LEVEL_WRITE )
        {
            waitResult = ::WaitForSingleObject( m_unlockEvent, timeout );
            if ( waitResult == WAIT_OBJECT_0 )
            {
                guard gd(&state_change_mutex_);

                m_currentLevel = level;
                m_readCount += 1;
                ::ResetEvent( m_unlockEvent );
            }
            else bresult = false;
        }
        else if ( level == LOCK_LEVEL_WRITE && m_currentLevel == LOCK_LEVEL_NONE )
        {
            guard gd(&state_change_mutex_);

            m_currentLevel = level;
            ::ResetEvent( m_unlockEvent );
        }
        else if ( level == LOCK_LEVEL_WRITE && m_currentLevel != LOCK_LEVEL_NONE )
        {
            waitResult = ::WaitForSingleObject( m_unlockEvent, timeout );
            if ( waitResult == WAIT_OBJECT_0 )
            {
                guard gd(&state_change_mutex_);

                m_currentLevel = level;
                ::ResetEvent( m_unlockEvent );
            }
            else bresult = false;
        }
        
        ::ReleaseMutex( m_accessMutex );
        return bresult;
        
    } // lock()
	
	bool unlock()
    {
        guard gd(&state_change_mutex_);

        if ( m_currentLevel == LOCK_LEVEL_READ )
        {
            m_readCount --;
            if ( m_readCount == 0 )
            {
                m_currentLevel = LOCK_LEVEL_NONE;
                ::SetEvent (m_unlockEvent);
            }
        }
        else if ( m_currentLevel == LOCK_LEVEL_WRITE )
        {
            m_currentLevel = LOCK_LEVEL_NONE;
            ::SetEvent ( m_unlockEvent );
        }
        
        return true;
    }

public:
    ///< ×Ô¶¯¼ÓËø£¬½âËøÀà
    class auto_lock_read
    {
    public:
        auto_lock_read(read_write_lock& rwlock) : wrlock_(rwlock) { wrlock_.lock(LOCK_LEVEL_READ); }
        ~auto_lock_read() { wrlock_.unlock(); }
    protected:
        read_write_lock& wrlock_;
    };

    class auto_lock_write
    {
    public:
        auto_lock_write(read_write_lock& rwlock) : wrlock_(rwlock) { wrlock_.lock(LOCK_LEVEL_WRITE); }
        ~auto_lock_write() { wrlock_.unlock(); }
    protected:
        read_write_lock& wrlock_;
    };
};


}}


#endif