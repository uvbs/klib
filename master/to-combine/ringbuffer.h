
#pragma once

namespace stats
{
	typedef size_t                              size_type;

/**
 * @brief  环型缓冲区
 * @tparam  TDataType  存储的类型
*/
template<class TDataType>
class CRingBuffer
{
public:
    typedef CRingBuffer<TDataType>              self_type;

public:
    CRingBuffer() : m_nMaxSize(0), m_nSize(0), m_nWritePos(0), m_nReadPos(0), m_pDataArr(NULL) {}
    ~CRingBuffer() 
    {
        if (m_pDataArr) 
        {
            delete[] m_pDataArr;
        }
    }

    bool set_max_size(size_type nmax) 
    {
        if (nmax > 0 && m_nMaxSize == 0) 
        {
            m_pDataArr = new TDataType[nmax];
            m_nMaxSize = nmax;
            if (NULL == m_pDataArr) 
            {
                m_nMaxSize = 0;
            }
            return m_pDataArr != NULL;
        }
        return true;
    }

    bool push_back(const TDataType& target)
    {
        if (m_nSize >= m_nMaxSize) 
            return false;

        _ASSERT(m_nSize <= m_nMaxSize);
        m_pDataArr[m_nWritePos] = target;
        ++ m_nSize;
        ++ m_nWritePos;
        m_nWritePos %= m_nMaxSize;

        return true;
    }

    bool push_force(const TDataType& target)
    {
        if (is_full()) 
        {
            pop_front();
        }
        return push_back(target);
    }
    
    bool front(TDataType& target)
    {
        if (0 == m_nSize) 
            return false;

        target = m_pDataArr[m_nReadPos];

        return true;
    }

    bool pop_front()
    {
        if (0 == m_nSize) 
            return false;

        ++ m_nReadPos;
        m_nReadPos %= m_nMaxSize;
        -- m_nSize;

        return true;
    }

    bool peek(TDataType& target) const
    {
        if (empty()) 
            return false;

        target = m_pDataArr[m_nReadPos];
        return true;
    }

    bool peek(TDataType*& pTarget) 
    {
        if (0 == m_nSize) 
        {
            pTarget = NULL;
            return false;
        }

        pTarget = &m_pDataArr[m_nReadPos];
        return true;
    }

    bool peek_back(TDataType*& pTarget) 
    {
        if (empty()) 
        {
            return false;
        }

        size_type uIndex = size() - 1;
        pTarget = & at(uIndex);
        return true;
    }

    TDataType& at(size_type uIndex) 
    {
        size_type uTmp = m_nReadPos + uIndex;
        uTmp %= m_nMaxSize;
        
        return m_pDataArr[uTmp];
    }

    size_type size() const
    {
        return m_nSize;
    }

    bool is_full() const
    {
        return m_nSize == m_nMaxSize;
    }

    bool empty() const
    {
        return 0 == m_nSize;
    }

	size_t max_size() const
	{
		return m_nMaxSize;
	}

protected:
    size_type m_nMaxSize;
    size_type m_nSize;
    size_type m_nWritePos;
    size_type m_nReadPos;
    TDataType* m_pDataArr;
};


//----------------------------------------------------------------------
// 有锁缓冲区
template<class TDataType>
class CLockRingBuffer : public CRingBuffer<TDataType>
{
public:
    bool set_max_size(size_type nmax) 
    {
        CCSAutoLock lock(&m_objAutoCS);

        return __super::set_max_size(nmax);
    }

    bool push_back(const TDataType& target)
    {
        CCSAutoLock lock(&m_objAutoCS);

        return __super::push_back(target);
    }

    bool front(TDataType& target)
    {
        CCSAutoLock lock(&m_objAutoCS);

        return __super::front(target);
    }

    bool pop_front()
    {
        CCSAutoLock lock(&m_objAutoCS);

        return __super::pop_front();
    }

    bool peek(TDataType& target) 
    {
        CCSAutoLock lock(&m_objAutoCS);

        return __super::peek(target);
    }

    bool peek(TDataType*& pTarget) 
    {
        CCSAutoLock lock(&m_objAutoCS);

        return __super::peek(pTarget);
    }

    bool peek_back(TDataType*& pTarget)  
    {
        CCSAutoLock lock(&m_objAutoCS);

        return __super::peek_back(pTarget);
    }

    TDataType at(size_type uIndex) 
    {
        CCSAutoLock lock(&m_objAutoCS);

        return __super::at(uIndex);
    }

    bool is_full() const 
    {
        return __super::is_full();
    }

protected:
    mutable CCSAutoCritSec m_objAutoCS;
};


}