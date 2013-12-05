#ifndef _RW_LOCK_h_
#define _RW_LOCK_h_


namespace klib {
namespace kthread {


#define LOCK_LEVEL_NONE 0
#define LOCK_LEVEL_READ 1           ///< ¶ÁËø
#define LOCK_LEVEL_WRITE 2          ///< Ð´Ëø


///< ¶ÁÐ´Ëø
class CReadWriteLock
{
protected:
    int    m_currentLevel;
    int    m_readCount;  
    HANDLE m_unlockEvent;
    HANDLE m_accessMutex;
    CRITICAL_SECTION m_csStateChange;
    
public:
    CReadWriteLock()
    {
        m_currentLevel = LOCK_LEVEL_NONE;
        m_readCount    = 0;
        m_unlockEvent  = ::CreateEvent( NULL, TRUE, FALSE, NULL );
        m_accessMutex  = ::CreateMutex( NULL, FALSE, NULL );
        ::InitializeCriticalSection( &m_csStateChange );
    }
    ~CReadWriteLock()
    {
        ::DeleteCriticalSection( &m_csStateChange );
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
            ::EnterCriticalSection( &m_csStateChange );
            m_currentLevel = level;
            m_readCount += 1;
            ::ResetEvent( m_unlockEvent );
            ::LeaveCriticalSection( &m_csStateChange );
        }
        else if ( level == LOCK_LEVEL_READ && m_currentLevel == LOCK_LEVEL_WRITE )
        {
            waitResult = ::WaitForSingleObject( m_unlockEvent, timeout );
            if ( waitResult == WAIT_OBJECT_0 )
            {
                ::EnterCriticalSection( &m_csStateChange );
                m_currentLevel = level;
                m_readCount += 1;
                ::ResetEvent( m_unlockEvent );
                ::LeaveCriticalSection( &m_csStateChange );
            }
            else bresult = false;
        }
        else if ( level == LOCK_LEVEL_WRITE && m_currentLevel == LOCK_LEVEL_NONE )
        {
            ::EnterCriticalSection( &m_csStateChange );
            m_currentLevel = level;
            ::ResetEvent( m_unlockEvent );
            ::LeaveCriticalSection( &m_csStateChange );
        }
        else if ( level == LOCK_LEVEL_WRITE && m_currentLevel != LOCK_LEVEL_NONE )
        {
            waitResult = ::WaitForSingleObject( m_unlockEvent, timeout );
            if ( waitResult == WAIT_OBJECT_0 )
            {
                ::EnterCriticalSection( &m_csStateChange );
                m_currentLevel = level;
                ::ResetEvent( m_unlockEvent );
                ::LeaveCriticalSection( &m_csStateChange );
            }
            else bresult = false;
        }
        
        ::ReleaseMutex( m_accessMutex );
        return bresult;
        
    } // lock()
	
	bool unlock()
    {
        ::EnterCriticalSection( &m_csStateChange );
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
        ::LeaveCriticalSection( &m_csStateChange );
        
        return true;
    }

public:
    ///< ×Ô¶¯¼ÓËø£¬½âËøÀà
    class CAutoLockRead
    {
    public:
        CAutoLockRead(CReadWriteLock& rwlock) : m_wrlock(rwlock) { m_wrlock.lock(LOCK_LEVEL_READ); }
        ~CAutoLockRead() { m_wrlock.unlock(); }
    protected:
        CReadWriteLock& m_wrlock;
    };

    class CAutoLockWrite
    {
    public:
        CAutoLockWrite(CReadWriteLock& rwlock) : m_wrlock(rwlock) { m_wrlock.lock(LOCK_LEVEL_WRITE); }
        ~CAutoLockWrite() { m_wrlock.unlock(); }
    protected:
        CReadWriteLock& m_wrlock;
    };
};


}}


#endif