// Queue.h: interface for the Queue class.
//
//	1对n lock free队列
//	并发模式：1读n写、n读1写
//	push与pop复杂度：O(1)
//////////////////////////////////////////////////////////////////////

#ifndef MDK_QUEUE_H
#define MDK_QUEUE_H


#include <stdint.h>
#include <windows.h>
#include <kthread/atomic.h>


#define  AtomDec  InterlockedDecrement
#define  AtomAdd  InterlockedIncrement  

namespace klib {
namespace stl {
namespace lockfree {

template<class T, int max_queue_size>
class one_many_queue
{
    typedef	struct QUEUE_NODE
    {
        T    object;
        bool IsEmpty;
    } QUEUE_NODE;

public:
    ~one_many_queue()
    {
    }

public:
    one_many_queue()
    {
        m_nSize = max_queue_size;
        m_nWriteAbleCount = m_nSize;
        m_nReadAbleCount = 0;
        m_push = 0;
        m_pop = 0;

        clear();
    }

    LONG size() { return m_nReadAbleCount; }

    bool push(const T& object )
    {
        if ( 0 >= m_nWriteAbleCount ) return false;//列队已满
        if ( 0 > AtomDec(&m_nWriteAbleCount) ) //已经有足够多的push操作向列队不同位置写入数据
        {
            AtomAdd(&m_nWriteAbleCount);
            return false;
        }
        uint32_t pushPos = AtomAdd(&m_push);
        pushPos = pushPos % m_nSize;
        /*
        只有在NPop并发情况下，因Pop无序完成，第一个位置的Pop未完成，
        后面的Pop就先完成提示有空位，导致这里检查第一个位置时，发现数据未被取走
        因为该类只允许1对N，所以必然是单线程Push( void *pObject )，所以条件内m_push不需要并发控制
        */
        if (!m_queue[pushPos].IsEmpty ) 
        {
            m_push--;
            AtomAdd(&m_nWriteAbleCount);
            return false;
        }
        m_queue[pushPos].object = object;
        m_queue[pushPos].IsEmpty = false;
        AtomAdd(&m_nReadAbleCount);

        return true;
    }

    bool pop(T& t)
    {
        if ( 0 >= m_nReadAbleCount ) return NULL;//空列队
        if ( 0 > AtomDec(&m_nReadAbleCount)) //已经有足够多的pop操作读取列队不同位置的数据
        {
            AtomAdd(&m_nReadAbleCount);
            return false;
        }
        uint32_t popPos = AtomAdd(&m_pop);
        popPos = popPos % m_nSize;
        /*
        只有在NPush并发情况下，因Push无序完成，第一个位置的Push未完成，
        后面的Push就先完成提示有数据，导致这里检查第一个位置时，发现没有数据可读
        因为该类只允许1对N，必然是单线程Pop()，所以条件内m_pop不需要并发控制
        */
        if ( m_queue[popPos].IsEmpty )
        {
            m_pop--;
            AtomAdd(&m_nReadAbleCount);
            return false;
        }

        t = m_queue[popPos].object;
        //m_queue[popPos].pObject = NULL;
        m_queue[popPos].IsEmpty = true;
        AtomAdd(&m_nWriteAbleCount);

        return true;
    }

    void clear()
    {
        if ( NULL == m_queue ) return;
        uint32_t i = 0;
        m_nWriteAbleCount = m_nSize;
        m_nReadAbleCount = 0;
        for ( i = 0; i < m_nSize; i++ )
        {
            //m_queue[i].object = NULL;
            m_queue[i].object.~T();
            m_queue[i].IsEmpty = true;
        }
    }

private:
    QUEUE_NODE m_queue[max_queue_size];

    uint32_t m_push;
    uint32_t m_pop;
    uint32_t m_nSize;
    volatile LONG m_nWriteAbleCount;
    volatile LONG m_nReadAbleCount;
};


}}}

#endif // MDK_QUEUE_H
