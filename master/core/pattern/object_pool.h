#ifndef _klib_object_pool_h
#define _klib_object_pool_h

#pragma pack(push,_CRT_PACKING)
#pragma warning(push,3)
#pragma push_macro("new")
    #undef new


#include <vector>
#include "..\kthread\auto_lock.h"

namespace klib {
namespace pattern {

using namespace klib::kthread;


template<class T>
class CMemBlock
{
public:
    CMemBlock() : bFixed(FALSE), uIndex(0), pNext(NULL) {}

    T               obj;        // 承载的对象

    BOOL            bFixed;     // 固定的话表示是需要被delete[]掉的
    size_t          uIndex;     // 在vector中的索引
    CMemBlock*      pNext;      // 指向下一个内存块
};

//默认保留的桶的大小
#define  DEFAULT_MEM_BLOCK_RESERVE      (50)

/**
 * @brief  对象池
 * @tparam  INIT_SIZE 初始大小
 * @tparam  INC_SIZE  增长大小
 */
template<class T, INT INIT_SIZE, INT INC_SIZE>
class CObjectPool
{
public:
    //确定内存池的初始化大小以及增长的大小
    CObjectPool() 
    {
        m_vecMemBlock.reserve(DEFAULT_MEM_BLOCK_RESERVE);

        AllocBlocks();
    };

    ~CObjectPool()
    {
        FreeAll();
    }

    T*      Alloc();
    void    Free(T* pobj);                       //针对申请的对象指针来释放
    void    FreeAll();                           //释放所有的内存

    void    ReleaseSome();                       //查看并释放能够释放的内存,外部调用

private:
    void    AllocBlocks(int iCount = 0);     //创建内存块
    void    ReAllocBlocks();                 //内存块不足时重新创建
    
    bool    Empty();                         //判断内存池是否为空

private:
    struct stMemBlockInfo 
    {
        stMemBlockInfo() : uWholeBlock(0), uFreeBlock(0), pHead(NULL), pFixedBlock(NULL) {}

        UINT    uWholeBlock;
        UINT    uFreeBlock;
        CMemBlock<T> * pHead;               // 内存块链表的开始位置，不一定是块的头部
        CMemBlock<T> * pFixedBlock;         // 用于快速释放内存的
    };

    auto_cs         m_obj_auto_cs;
    
    typedef std::vector<stMemBlockInfo> MemBlockVecType;
    MemBlockVecType         m_vecMemBlock;
};

template<class T, INT INIT_SIZE, INT INC_SIZE>
void CObjectPool<T, INIT_SIZE, INC_SIZE>::AllocBlocks(int iCount/* = initSize*/)
{
    if (iCount == 0) {
        iCount = INIT_SIZE;
    }
    
    CMemBlock<T>* pBuff = new CMemBlock<T>[iCount];
    if (NULL == pBuff) {
        return;
    }

    MemBlockVecType::size_type uIndex = 0;
    for (; uIndex < m_vecMemBlock.size(); ++ uIndex) {
        if (0 == m_vecMemBlock[uIndex].uWholeBlock) {
            break;
        }
    }
    if (uIndex == m_vecMemBlock.size()) {
        m_vecMemBlock.resize(uIndex + 1);
        _ASSERT(m_vecMemBlock.size() == uIndex + 1);
    }

    CMemBlock<T>* pHead = NULL;
    CMemBlock<T>* pPos = pBuff;
    for (int i=0; i < iCount - 1; ++ i) {
        pPos->uIndex = uIndex;

        pPos->pNext = &pBuff[i+1];
        pPos = pPos->pNext;
    }
    pPos->uIndex = uIndex;
    pPos->pNext = pHead;

    pHead = pBuff;
    pHead->bFixed = TRUE;

    stMemBlockInfo info;
    info.uFreeBlock = info.uWholeBlock = iCount;
    info.pHead = pHead;
    info.pFixedBlock = pHead;
    m_vecMemBlock[uIndex] = info;
}

template<class T, INT INIT_SIZE, INT INC_SIZE>
void CObjectPool<T, INIT_SIZE, INC_SIZE>::ReAllocBlocks()
{
    AllocBlocks(INC_SIZE);
}

template<class T, INT INIT_SIZE, INT INC_SIZE>
bool CObjectPool<T, INIT_SIZE, INC_SIZE>::Empty()
{
    MemBlockVecType::const_iterator itr;
    itr = m_vecMemBlock.begin();
    for (; itr != m_vecMemBlock.end(); ++ itr) {
        if (itr->pHead != NULL) {
            return false;
        }
    }
    
    return true;
}

template<class T, INT INIT_SIZE, INT INC_SIZE>
T*  CObjectPool<T, INIT_SIZE, INC_SIZE>::Alloc()
{
    auto_lock lock(m_obj_auto_cs);

    if (Empty()) {
        ReAllocBlocks();
    }

    CMemBlock<T>* pTmp = NULL;
    MemBlockVecType::iterator itr = m_vecMemBlock.begin();
    for (; itr != m_vecMemBlock.end(); ++ itr) {
        if (NULL != itr->pHead) {
            pTmp = itr->pHead;
            itr->pHead = itr->pHead->pNext;

            -- itr->uFreeBlock;
            break;
        }
    }
    
    return (T*) pTmp;
}

template<class T, INT INIT_SIZE, INT INC_SIZE>
void CObjectPool<T, INIT_SIZE, INC_SIZE>::Free(T* pobj)
{
    _ASSERT(pobj);

    // 重新构造一个结构

    CMemBlock<T>* pos = (CMemBlock<T>*) ( pobj);
    pos->~CMemBlock<T>();
    new (pobj) CMemBlock<T>;

    auto_lock lock(m_obj_auto_cs);

    stMemBlockInfo& info = m_vecMemBlock[pos->uIndex];
    CMemBlock<T>* & pHead = info.pHead;

    pos->pNext = pHead;
    pHead = pos;

    ++ info.uFreeBlock;
}

template<class T, INT INIT_SIZE, INT INC_SIZE>
void CObjectPool<T, INIT_SIZE, INC_SIZE>::FreeAll()
{
    MemBlockVecType::const_iterator itr;
    itr = m_vecMemBlock.begin();

    for (; itr != m_vecMemBlock.end(); ++ itr) 
    {
        if (0 == itr->uWholeBlock) 
        {
            continue;
        }

        CMemBlock<T> *pFixedBlock = itr->pFixedBlock;
        delete[] pFixedBlock;
    }
}

template<class T, INT INIT_SIZE, INT INC_SIZE>
void CObjectPool<T, INIT_SIZE, INC_SIZE>::ReleaseSome()
{
    auto_lock lock(m_obj_auto_cs);

    MemBlockVecType::size_type uIndex = 1;
    while (uIndex < m_vecMemBlock.size())
    {
        stMemBlockInfo& info = m_vecMemBlock[uIndex];

        if (info.uWholeBlock == info.uFreeBlock) 
        {
            //可以释放

            delete[] info.pFixedBlock;
            
            new (&info) stMemBlockInfo;
        }

        ++ uIndex;
    }
}



}}



#pragma pop_macro("new")
#pragma warning(pop)
#pragma pack(pop)


#endif
