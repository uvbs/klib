#ifndef _klib_lock_h_
#define _klib_lock_h_



 // 同步对象
class auto_cs
{
public:
    auto_cs() {  InitializeCriticalSection(&m_cs);  }
    ~auto_cs() {  DeleteCriticalSection(&m_cs);    }

    void lock() {  EnterCriticalSection(&m_cs);  }
    void unlock()  {  LeaveCriticalSection(&m_cs);  }

protected:
    auto_cs(const auto_cs&);
    auto_cs& operator = (const auto_cs& rhs);

protected:
    CRITICAL_SECTION m_cs;
};

// 同步锁对象
class auto_lock
{
public:
    explicit auto_lock(auto_cs& cs) : m_cs(cs)  {  m_cs.lock();  }
    ~auto_lock()  {  m_cs.unlock();  }

protected:
    auto_cs& m_cs;
};



#endif // _klib_lock_h_