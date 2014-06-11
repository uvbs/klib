#ifndef _klib_fix_circle_buff_h
#define _klib_fix_circle_buff_h



#include "../kthread/auto_lock.h"


namespace klib {
namespace pattern {

using namespace klib::kthread;

/**
 * @brief  循环缓冲区
 * @tparam  T  存储的类型
 * @tparam  LenType 内部状态表示的长度,主要是为了节约内存，可以用不同大小的定义
 * @tparam  nBuffLen 缓冲区长度
*/
template<class T, class LenType, INT nBuffLen>
class fix_circle_buff
{
public:
    fix_circle_buff() : m_nSize(0), m_nWritePos(0), m_nReadPos(0) {}

    bool push_item(const T& target)
    {
        if (m_nSize >= nBuffLen) 
        {
            return false;
        }

        _ASSERT(m_nSize <= nBuffLen);
        m_val[m_nWritePos] = target;
        ++ m_nSize;
        ++ m_nWritePos;
        m_nWritePos %= nBuffLen;

        return true;
    }

    bool front(T& target)
    {
        if (0 == m_nSize) 
        {
            return false;
        }

        target = m_val[m_nReadPos];
        ++ m_nReadPos;
        m_nReadPos %= nBuffLen;
        -- m_nSize;

        return true;
    }

    bool pop_front()
    {
        if (0 == m_nSize) 
        {
            return false;
        }

        ++ m_nReadPos;
        m_nReadPos %= nBuffLen;
        -- m_nSize;

        return true;
    }

    bool peek(T& target)
    {
        if (0 == m_nSize) 
        {
            return false;
        }

        target = m_val[m_nReadPos];
        return true;
    }

    bool peek(T*& pTarget)
    {
        if (0 == m_nSize) 
        {
            pTarget = NULL;
            return false;
        }

        pTarget = &m_val[m_nReadPos];
        return true;
    }

    T& at(LenType uIndex)
    {
        LenType uTmp = m_nReadPos + uIndex;
        uTmp %= nBuffLen;

        if (uTmp == m_nWritePos) 
        {
            throw std::exception("error index");
            return m_val[0];
        }

        return m_val[uTmp];
    }

    UINT size()
    {
        return m_nSize;
    }

    bool is_full()
    {
        return m_nSize == nBuffLen;
    }

protected:
    LenType m_nSize;
    LenType m_nWritePos;
    LenType m_nReadPos;
    T m_val[nBuffLen];
};


//----------------------------------------------------------------------
// 有锁缓冲区
template<class T, class LenType, INT nBuffLen>
class CLockFixedBuff : public fix_circle_buff<T, LenType, nBuffLen>
{
public:
    bool push_item(const T& target)
    {
        auto_lock lock(m_objAutoCS);

        return m_objBuffer.Push(target);
    }

    bool front(T& target)
    {
        CAutoLock lock(m_objAutoCS);

        return m_objBuffer.front(target);
    }

    bool pop_front()
    {
        auto_lock lock(m_objAutoCS);

        return m_objBuffer.pop_front();
    }

    bool peek(T& target)
    {
        auto_lock lock(m_objAutoCS);

        return peek(target);
    }

    bool peek(T*& pTarget)
    {
        auto_lock lock(m_objAutoCS);

        return peek(pTarget);
    }

    T* at(LenType uIndex)
    {
        auto_lock lock(m_objAutoCS);

        return at(uIndex);
    }

protected:
    auto_cs m_objAutoCS;
    fix_circle_buff<T, LenType, nBuffLen> m_objBuffer;
};



}}



#endif
