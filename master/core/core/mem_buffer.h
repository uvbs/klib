#pragma once

#include "..\algo\bm_wrapper.h"
#include <vector>

namespace klib {
namespace mem{

using klib::algo::bm_wrapper;


/**
 * @brief   内存缓冲区(分段内存)
 *          主要为节约内存
 *          
 * @code
 *          mem_buffer<1024, 20> buff;
 *          buff.Append("contents", 8);
 *      
 * @endcode
 * 
 * @tparam  MAX_BUFFER_SIZE the maximum buffer size.
 * @tparam  MIN_BLOCK_SIZE  the minimum block size.
 * @tparam  bUseStaticMemory 是否使用静态内存（如果使用，需要先初始化InitStaticMemory）
 */
template<int MAX_BUFFER_SIZE, int MIN_BLOCK_SIZE, BOOL bUseStaticMemory>
class mem_buffer
{
public:
    #define  MAX_BUCKER_SIZE    ((MAX_BUFFER_SIZE / MIN_BLOCK_SIZE) + ((MAX_BUFFER_SIZE % MIN_BLOCK_SIZE) > 0))          ///< 最大的桶的个数
    
    typedef std::list<char*> MemoryListType;
    typedef size_t           size_type;
    typedef mem_buffer<MAX_BUFFER_SIZE, MIN_BLOCK_SIZE, bUseStaticMemory>   self_type;

public:
    mem_buffer(void) :
        m_ncontent_size(0),
        m_nbuffer_size(0),
        m_ncur_bucket_pos(0),
        m_ncur_bucket_index(0)
    {
        static_assert(MAX_BUFFER_SIZE >= MIN_BLOCK_SIZE, "buffer size must big than block");
    }

    ~mem_buffer(void)
    {
        clear();
    }

    /**
     * @brief   仅释放一部分内存，并不会将该对象删除.
     */
    void clear()
    {
        for (auto itr=m_buckets_vec.begin(); itr != m_buckets_vec.end(); ++itr) 
        {
            reuse_memory(*itr);
        }
        m_buckets_vec.clear();

        InterlockedExchangeAdd64(&get_whole_buffer_size(), - (INT64)m_nbuffer_size);

        m_ncontent_size = 0;
        m_nbuffer_size = 0;
        m_ncur_bucket_pos = 0;
        m_ncur_bucket_index = 0;
    }

    /**
     * @brief   判断缓冲区是否为空
     */
    bool empty() const
    {
        return m_ncontent_size == 0;
    }

    /**
     * @brief   判断缓冲区是否相等
     */
    bool operator == (self_type& other) const
    {
        if (size() != other.size()) 
        {
            return false;
        }
        if (empty())
        {
            return true;
        }

        size_t nbucket_index = size() / MIN_BLOCK_SIZE;
        size_t nbucket_pos   = size() % MIN_BLOCK_SIZE;

        size_type npos = 0;
        for (; npos < nbucket_index; ++ npos)
        {
            if (memcmp(m_buckets_vec[npos], other.m_buckets_vec[npos], MIN_BLOCK_SIZE) != 0) 
            {
                return false;
            }
        }

        return  (memcmp(m_buckets_vec[npos], other.m_buckets_vec[npos], nbucket_pos) == 0) ;
    }
    
    /**
     * @brief  添加数据到缓冲区中
     * @param  [in]  pszBuff       要添加数据的缓冲区
     * @param  [in]  nLen          要添加缓冲区的长度
     * @return true  已将数据添加到缓冲区
     *         false 添加失败，已达到最大缓冲区或者，达到设定的限制值
     */
    bool append(const char* pszBuff, size_t nLen)
    {
        // 不能超过最大定义的缓冲区大小
        if (m_ncontent_size + nLen > MAX_BUFFER_SIZE) 
        {
            return false;
        }

        if (m_buckets_vec.empty()) 
        {
            char* p = alloc();
            if (NULL == p) 
            {
                return false;
            }

            m_buckets_vec.push_back(p);
            _ASSERT(m_buckets_vec.size() == 1);

            m_nbuffer_size   = MIN_BLOCK_SIZE;
            InterlockedExchangeAdd64(&get_whole_buffer_size(), MIN_BLOCK_SIZE);
        }

        size_t nCopyExistLen = nLen;
        size_t nBucketExists = MIN_BLOCK_SIZE - m_ncur_bucket_pos;
        const char* pCurPos = pszBuff;;

        while (nCopyExistLen >= nBucketExists)
        {
            size_t nCurBucketCopySize = nBucketExists;
            memcpy(&m_buckets_vec[m_ncur_bucket_index][m_ncur_bucket_pos], pCurPos, nCurBucketCopySize);
            pCurPos += nCurBucketCopySize;

            nCopyExistLen -= nBucketExists;
            m_ncontent_size += nCurBucketCopySize;
            m_ncur_bucket_pos += nCurBucketCopySize;
            nBucketExists = 0;

            if (m_buckets_vec.size() <= m_ncur_bucket_index + 1) 
            {
                auto pBuff = alloc();
                if (NULL == pBuff) 
                {
                    return false;
                }
                InterlockedExchangeAdd64(&get_whole_buffer_size(), MIN_BLOCK_SIZE);
                m_buckets_vec.push_back(pBuff);
                m_nbuffer_size += MIN_BLOCK_SIZE;
            }

            ++ m_ncur_bucket_index;
            m_ncur_bucket_pos = 0;
            nBucketExists = MIN_BLOCK_SIZE;
        }

        memcpy(&m_buckets_vec[m_ncur_bucket_index][m_ncur_bucket_pos], pCurPos, nCopyExistLen);
        m_ncur_bucket_pos += nCopyExistLen;
        m_ncontent_size += nCopyExistLen;
        
        return true;
    }

    /**
     * @brief  将str数据添加到缓冲中
     * @param  [in]  str   保存要添加的数据内容
     * @return true  添加到缓冲区中
     *         false 未添加成功
     */
    bool append(const std::string& str)
    {
        return append(str.c_str(), str.size());
    }

    /**
     * @brief 将缓冲区的数据添加到当前缓冲区中
     */
    bool append(const mem_buffer& s)
    {
        if (s.empty()) 
        {
            return true;
        }

        size_t nbucket_index = s.size() / MIN_BLOCK_SIZE;
        size_t nbucket_pos   = s.size() % MIN_BLOCK_SIZE;

        size_t npos = 0;
        for (; npos < nbucket_index; ++ npos) 
        {
            this->append(s.m_buckets_vec[npos], MIN_BLOCK_SIZE);
        }
        return this->append(s.m_buckets_vec[npos], nbucket_pos);
    }


    mem_buffer& operator << (const char* psrc)
    {
        append(psrc);
        return *this;
    }

    /**
     * @brief  将缓冲区中的数据提取到pbuff中
     * @param  [in,out]  pbuff    要被提取的缓冲区
     * @param  [in]      nPos     在缓冲区中的起始位置
     * @param  [in]      nLen     要提取内容的长度
     * @return true  数据提取成功
     *         false 数据提取失败，缓冲区中的内容长度不够
     */
    bool copy(__inout char* pbuff, size_t nPos, size_t nLen) const
    {
        _ASSERT(pbuff);
        // 参数验证
        if (NULL == pbuff || 0 == nLen) 
        {
            return false;
        }

        // 判断是否有足够的空间
        if ((nPos + nLen) > m_ncontent_size) 
        {
            return false;
        }

        char* pFetechPos = pbuff;
        size_t nbucket_index = nPos / MIN_BLOCK_SIZE;
        size_t nbucket_pos = nPos % MIN_BLOCK_SIZE;
        size_t nFetechExist = nLen;
        size_t nBucketExistLen = MIN_BLOCK_SIZE - nbucket_pos; 
        while (nFetechExist > nBucketExistLen)
        {
            memcpy(pFetechPos, &m_buckets_vec[nbucket_index][nbucket_pos], nBucketExistLen);

            pFetechPos += nBucketExistLen;
            nFetechExist -= nBucketExistLen;

            ++ nbucket_index;
            nbucket_pos = 0;
            nBucketExistLen = MIN_BLOCK_SIZE - nbucket_pos;
        }
        memcpy(pFetechPos, &m_buckets_vec[nbucket_index][nbucket_pos], nFetechExist);

        return true;
    }

    /**
     * @brief  查找字符串
     * @param  [in]  pszStr  要被查找的字符串
     * @param  [in]  nLen    要查找字符串的长度
     * @param  [in]  nStartPos 查找的起始位置
     * @param  [in]  bCaseSence 大小写敏感
     * @return  -1   查找失败
     *          >=0  返回的是在缓冲区的起始位置
     */
    int find(const char* pszStr, size_t nLen, size_t nStartPos, bool bCaseSense) const
    {
        if (nStartPos + nLen > m_ncontent_size) 
        {
            return -1;
        }
        if (nLen > m_ncontent_size) 
        {
            return -1;
        }
        if (nStartPos > m_ncontent_size) 
        {
            return -1;
        }

        size_t nbucket_index = nStartPos / MIN_BLOCK_SIZE;
        size_t nbucket_pos   = nStartPos % MIN_BLOCK_SIZE;
        if (0 == nLen) 
        {
            return -1;
        }

        bm_wrapper bm;
        bm.init_pattern((unsigned char*)pszStr, (int)nLen, bCaseSense);
        int nPosFind = -1;
        auto cmpFun = strncmp;
        if (!bCaseSense) 
        {
            cmpFun = _strnicmp;
        }

        while (nbucket_index < m_ncur_bucket_index)
        {
            // 行内查找
            nPosFind = bm.search((const unsigned char*) &m_buckets_vec[nbucket_index][nbucket_pos], (int)(MIN_BLOCK_SIZE - nbucket_pos));
            if (nPosFind != -1) 
            {
                return (int)(nbucket_index * MIN_BLOCK_SIZE) + nPosFind + (int)nbucket_pos;
            }
            else 
            {
                // 跨2个段查找
                if (nLen <= MIN_BLOCK_SIZE) 
                {
                    size_t nSearchLen = nLen - 1;  ///< 第一个段需要搜索的长度

                    for (;nSearchLen > 0; -- nSearchLen) 
                    {
                        // 第一个段剩余的空间要大于待搜索的长度
                        if (MIN_BLOCK_SIZE - nbucket_pos >= nSearchLen) 
                        {
                            size_t nTmpPos = MIN_BLOCK_SIZE - nSearchLen;   ///< 临时找查的位置
                            
                            // 第一个段比较nSearchLen长度的字符串,第二个段查找nLen - nSearchLen个字符串
                            if (cmpFun(&m_buckets_vec[nbucket_index][nTmpPos], pszStr, nSearchLen) == 0 && 
                                cmpFun(m_buckets_vec[nbucket_index+1], pszStr + nSearchLen, nLen - nSearchLen) == 0) 
                            {
                                return (int)(nbucket_index * MIN_BLOCK_SIZE) + (int)nTmpPos;;
                            }
                        }
                    }
                }
                else
                {
                    // 跨越3个及以上段的查找
                    size_t nFirstSearchLen = (MIN_BLOCK_SIZE - nbucket_pos);
                    size_t nLeftSearchLen = nLen;       ///< 剩余搜索的长度
                    size_t nSearchIndex = 0;
                    size_t nExistContentLen = 0;        ///< 剩下的内容长度
                    size_t nSkipedBlockSize = (nbucket_index * MIN_BLOCK_SIZE); ///< 跳过的块的大小，优化计算量
                    auto pCurPos = pszStr;

                    nExistContentLen = m_ncontent_size - (nSkipedBlockSize + (MIN_BLOCK_SIZE - nFirstSearchLen));
                    
                    // 第一次搜索的长度要大于0，并且剩余的空间足够进行搜索,
                    while (nFirstSearchLen > 0 
                           && nbucket_index < m_ncur_bucket_index ///< 搜索的索引必须要小于当前缓冲区的索引
                           && nExistContentLen >= nLen
                           )
                    {
                        size_t ntmp = MIN_BLOCK_SIZE - nFirstSearchLen;
                        nLeftSearchLen = nLen - nFirstSearchLen;

                        // 第一个区域的剩余长度要大于第一次搜索的长度
                        if (MIN_BLOCK_SIZE - nbucket_pos >= nFirstSearchLen) 
                        {
                            pCurPos = pszStr;
                            if (cmpFun(&m_buckets_vec[nbucket_index][ntmp], pCurPos, nFirstSearchLen) == 0) 
                            {
                                // 第一个区域匹配成功，需要匹配下面的区段
                                pCurPos += nFirstSearchLen;         ///< 待比较的字符串顺移
                                nSearchIndex = nbucket_index + 1;   ///< 跳到下一个段

                                // 小于当前总索引的时候整块比较（剩下的长度大于整块,搜索的索引小于当前最大索引,避免越界）
                                while (nLeftSearchLen > MIN_BLOCK_SIZE && nSearchIndex < m_ncur_bucket_index)
                                {
                                    if (cmpFun(&m_buckets_vec[nSearchIndex][0], pCurPos, MIN_BLOCK_SIZE) != 0) 
                                    {
                                        break;
                                    }

                                    nLeftSearchLen -= MIN_BLOCK_SIZE;
                                    pCurPos += MIN_BLOCK_SIZE;
                                    ++ nSearchIndex;
                                }

                                _ASSERT(nLeftSearchLen > 0);
                                // 如果 nLeftSearchLen > MIN_BLOCK_SIZE那么肯定没有找到
                                if (nLeftSearchLen <= MIN_BLOCK_SIZE) 
                                {
                                    // 找到查找的字符串
                                    if (cmpFun(&m_buckets_vec[nSearchIndex][0], pCurPos, nLeftSearchLen) == 0) 
                                    {
                                        return (int)(nbucket_index * MIN_BLOCK_SIZE) + (int)ntmp;
                                    }
                                }
                            }
                        }

                        -- nFirstSearchLen;
                        nExistContentLen = m_ncontent_size - (nSkipedBlockSize + (MIN_BLOCK_SIZE - nFirstSearchLen));
                    }
                }
            }

            nbucket_pos = 0;
            ++ nbucket_index;
        }

        _ASSERT(nbucket_index == m_ncur_bucket_index);
        nPosFind = bm.search((const unsigned char*) &m_buckets_vec[nbucket_index][nbucket_pos], (int)(m_ncur_bucket_pos - nbucket_pos));
        if (nPosFind == -1) 
        {
            return -1;
        }
        return (int)(m_ncur_bucket_index * MIN_BLOCK_SIZE) + (int)nbucket_pos + nPosFind;
    }

    /**
     * @brief  是否是以指定字符串开始
     * @param  [in]  pszStr   要判断的字符串
     * @param  [in]  nLen     要判断的字符串长度
     * @param  [in]  bCaseSence 是否大小写敏感
     * @return  true 是以指定的字符串开始
     *          false 不是
     */
    bool is_begin_with(const char* pszStr, size_t nLen, bool bCaseSence = true) const 
    {
        _ASSERT(pszStr);
        if (NULL == pszStr) 
        {
            return false;
        }

        auto cmpFun = strncmp;
        if (!bCaseSence) 
        {
            cmpFun = _strnicmp;
        }

        if (cmpFun(m_buckets_vec[0], pszStr, nLen) == 0) 
        {
            return true;
        }
        return false;
    }

    /**
     * @brief 
     */
    bool is_begin_with(const char* pszStr, bool bCaseSence = true) const 
    {
        return is_begin_with(pszStr, strlen(pszStr));
    }

    /**
     * @brief  是否是以指定的字符串结束
     * @param  pszStr   要判断的字符串
     * @param  nLen     要判断的字符串长度
     * @return true     是以该字符中结束
     *         false    不是
     */
    bool is_end_with(const char* pszStr, size_t nLen) const 
    {
        _ASSERT(pszStr);
        if (NULL == pszStr || size() < nLen) 
        {
            return false;
        }

        size_t nMatched = 0;
        size_t nPos = size() - 1;
        auto pszCur = pszStr + nLen - 1;
        while (pszCur >= pszStr)
        {
            if (this->operator[](nPos) == (unsigned char) *pszCur) 
            {
                -- nPos;
                -- pszCur;
                ++ nMatched;
            }
            else
            {
                break;
            }
        }

        if (nMatched == nLen) 
        {
            return true;
        }
        return false;
    }

    /**
     * @brief  判断是否是以pstStr结束
     */
    bool is_end_with(const char* pszStr) const 
    {
        return is_end_with(pszStr, strlen(pszStr));
    }

    /**
     * @brief  删除后面的nLen字符
     * @param  [in]  nLen  要删除的长度
     * @return true 删除成功
     *         false 删除失败
     */     
    bool erase_back(size_t nLen)
    {
        // m_ncontent_size, m_ncur_bucket_pos,m_ncur_bucket_index
        _ASSERT(nLen > 0);
        if (nLen > m_ncontent_size) 
        {
            return false;
        }

        m_ncontent_size -= nLen;
        m_ncur_bucket_index = m_ncontent_size / MIN_BLOCK_SIZE;
        m_ncur_bucket_pos   = m_ncontent_size % MIN_BLOCK_SIZE;
        return true;
    }
    
    /**
     *  @brief 获取所有缓冲区的大小
     *  @note 保存所有buffer使用的空间总和
     *        (使用静态局部变量主要是为了方便使用,不用在外面初始静态成员)
     */
    static volatile LONGLONG&  get_whole_buffer_size()
    {
        static volatile LONGLONG s_nwhole_buffer_size = 0;
        return s_nwhole_buffer_size;
    }

    /**
     * @brief  获取当前缓冲区的内容长度 
     */
    size_t  size() const
    {
        return m_ncontent_size;
    }

    /**
     * @brief  获取当前缓冲的大小
     * @note   非静态成员，所以只能是当前使用的缓冲区长度
     */
    size_t  capacity() const
    {
        return m_nbuffer_size;
    }

    /**
     * @brief   获取指定偏移位置的字符
     * @param   [in]  nPos  指定位置的偏移
     * @return  -1  偏移超过界限,需判断是否越界（未越界的情况下也可能返回-1）
     *          >=0 指定偏移的数据
     */
    __forceinline unsigned char operator[](size_t nPos) const
    {
        if (nPos > m_ncontent_size) 
        {
            return 0x0ff;
        }

        size_t nbucket_index = nPos / MIN_BLOCK_SIZE;
        size_t nbucket_pos   = nPos % MIN_BLOCK_SIZE;

        return *(unsigned char*) &m_buckets_vec[nbucket_index][nbucket_pos];
    }

    /**
     * @brief   静态内容操作
     */
    __forceinline static void init_static_memory(UINT64 uMaxBufferSize)
    {
        if (bUseStaticMemory) 
        {
            InitializeCriticalSection(&get_memory_cs());

            MemoryListType& memoryList = get_memory_list();

            UINT64 uBlockCount = (uMaxBufferSize / MIN_BLOCK_SIZE) ;
            uBlockCount += (uMaxBufferSize % MIN_BLOCK_SIZE) > 0;
            char* pBuff = new char[uBlockCount * MIN_BLOCK_SIZE];
            if (NULL != pBuff) 
            {
                _ASSERT(NULL == get_alloc_addr());
                get_alloc_addr() = pBuff;
                char* pCurBuff = pBuff;
                for (int i=0; i<uBlockCount; ++i) 
                {
                    memoryList.push_back(pCurBuff);
                    pCurBuff += MIN_BLOCK_SIZE;
                }
            }
        }
    }

    /**
     * @brief 反初始化内存
     */
    __forceinline static void uninit_static_memory()
    {
        if (bUseStaticMemory) 
        {
            free_static_memory();

            DeleteCriticalSection(&get_memory_cs());
        }
    }

    /**
     * @brief  返回未使用的内存的大小
     * @note   指已分配中未使用的大小
     * @return 未使用的内存大小
     */
    __forceinline static UINT64 get_static_memory_size()
    {
        UINT64 uSize = 0;
        if (bUseStaticMemory) 
        {
            uSize = (MIN_BLOCK_SIZE * get_memory_list().size());
        }
        else 
        {
            _ASSERT(FALSE);
        }

        return uSize;
    }

    /**
     * @brief   将静态内存释放掉 
     */ 
    __forceinline static void free_static_memory()
    {
        if (bUseStaticMemory) 
        {
            EnterCriticalSection(&get_memory_cs());

            _ASSERT(get_alloc_addr() != NULL);

            if (NULL != get_alloc_addr()) 
            {
                delete get_alloc_addr();
            }
            get_memory_list().clear();

            LeaveCriticalSection(&get_memory_cs());
        }
    }

protected:
    /**
     * @brief  获取申请内存的指针
     * @return 申请内存的指针，申请内存后会被改写
     */
    __forceinline static char* & get_alloc_addr()
    {
        static char* s_pAllocAddr = NULL;
        return s_pAllocAddr;
    }

    /**
     * @brief  从内存池中获取指定大小的内存块使用
     */
    __forceinline static char* alloc()
    {
        char* p = NULL;
        if (bUseStaticMemory) 
        {
            EnterCriticalSection(&get_memory_cs());

            // 内存池中的内存使用完后就不会再进行申请了
            MemoryListType& memoryList = get_memory_list();
            if (!memoryList.empty()) 
            {
                p = memoryList.front();
                memoryList.pop_front();
            }

            LeaveCriticalSection(&get_memory_cs());
        }
        else 
        {
            p = new char[MIN_BLOCK_SIZE];
        }

        return p;
    }

    /**
     * @brief  将该内存块释放到内存池中 
     */
    __forceinline static void reuse_memory(char* p)
    {
        if (bUseStaticMemory) 
        {
            _ASSERT(p);
            EnterCriticalSection(&get_memory_cs());

            if (p) 
            {
                get_memory_list().push_back(p);
            }
            
            LeaveCriticalSection(&get_memory_cs());
        }
        else
        {
            delete p;
        }
    }

    /**
     * @brief  获取内存池表
     */
    __forceinline static MemoryListType& get_memory_list()
    {
        static MemoryListType _memory;
        return _memory;
    }

    /**
     * 获取内存池的互斥对象 
     */
    __forceinline static CRITICAL_SECTION& get_memory_cs()
    {
        static CRITICAL_SECTION _cs;
        return _cs;
    }
    
protected:
    size_t m_ncontent_size;                     ///< 内容的总长度
    size_t m_nbuffer_size;                      ///< 内存缓冲区的总长度

    size_t m_ncur_bucket_pos;                   ///< 在桶中的位置 
    size_t m_ncur_bucket_index;                 ///< 桶的索引

    typedef std::vector<char*>  bucket_vector_type;
    bucket_vector_type m_buckets_vec;           ///< 存放缓冲区指针的数组
};


//#include <fstream>
using namespace std;

/**
 * @brief   内存文件的封装.
 *
 * @tparam  MAX_BUFFER_SIZE  the maximum buffer size.
 * @tparam  MIN_BLOCK_SIZE   the minimum block size.
 * @tparam  bUseStaticMemory 是否使用静态内存（如果使用，需要先初始化）
 */
template<int MAX_BUFFER_SIZE, int MIN_BLOCK_SIZE, BOOL bUseStaticMemory>
class mem_file_wrapper : public mem_buffer<MAX_BUFFER_SIZE, MIN_BLOCK_SIZE, bUseStaticMemory>
{
public:
    mem_file_wrapper() :
      m_nOpPos(0)
    {}

    typedef  size_t  streamsize;
    typedef  mem_file_wrapper<MAX_BUFFER_SIZE, MIN_BLOCK_SIZE, bUseStaticMemory> self_type;
    enum seek_dir {way_begin, way_cur, way_end};

public:
    mem_file_wrapper& read (char* s, streamsize n)
    {
        if (Fetech(s, m_nOpPos, n))
        {
            m_nOpPos += n;
        }

        return *this;
    }

    mem_file_wrapper& write (const char* s, streamsize n)
    {
        if (append(s, n))
        {
            m_nOpPos += n;
        }

        return *this;
    }
        
    size_t tellg()
    {
        return m_nOpPos;
    }

    mem_file_wrapper& seekg (size_t pos)
    {
        m_nOpPos = pos;
        return *this;
    }

    mem_file_wrapper& seekg (int off, seek_dir way)
    {
        if (way == way_begin) 
        {
            _ASSERT(off >= 0 && off < (int) size());
            if (off >= 0 && off < (int) size()) 
            {
                m_nOpPos = 0 + off;
            }
        }
        else if (way == way_cur) 
        {
            auto nNewPos = m_nOpPos + off;
            _ASSERT(nNewPos >=0 && nNewPos < size());
            if (nNewPos >=0 && nNewPos < size()) 
            {
                m_nOpPos += off;
            }
        }
        else if (way == way_end) 
        {
            _ASSERT(off <= 0);
            if (off <= 0) 
            {
                m_nOpPos = size() + off;
            }
        }
        else 
        {
            _ASSERT(0);
        }

        return *this;
    }

    mem_file_wrapper& getline (char* s, streamsize n)
    {
        _ASSERT(s);

        size_t nReadedCount = 0;
        auto pCurPos = s;
        unsigned char uCh = 0;

        while (m_nOpPos < size() -1 && (uCh = this->operator[](m_nOpPos)) && nReadedCount < (size_t) n - 1)
        {
            // 判断\r\n的情况
            if (uCh == '\r' && this->operator[](m_nOpPos + 1) == '\n') 
            {
                m_nOpPos += 2;      // 需要跳过回车换行的地方
                break;
            }
            else if (uCh == '\n')  // 判断只有回车的情况
            {
                ++ m_nOpPos;
                break;
            }
            else
            {
                *pCurPos = uCh;
            }

            ++ pCurPos;
            ++ nReadedCount;
            ++ m_nOpPos;
        }
        *pCurPos = '\0';

        return *this;
    }

    mem_file_wrapper& getline (char* s, streamsize n, char delim)
    {
        _ASSERT(s);

        size_t nReadedCount = 0;
        auto pCurPos = s;
        unsigned char uCh = 0;

        while (m_nOpPos < size() -1 && (uCh = this->operator[](m_nOpPos)) && nReadedCount < n - 1)
        {
            if (uCh != delim) 
            {
                *pCurPos = uCh;
            }
            else 
            {
                ++ m_nOpPos;
                break;
            }

            ++ pCurPos;
            ++ nReadedCount;
            ++ m_nOpPos;
        }
        *pCurPos = '\0';

        return *this;
    }

    mem_file_wrapper& getline (char* s, streamsize n, char* szdelim)
    {
        _ASSERT(s && szdelim);

        size_t nReadedCount = 0;
        auto pCurPos = s;
        unsigned char uCh = 0;

        auto pCompareDelim = szdelim;
        size_t nComparePos = 0;
        size_t nDelimLen = strlen(szdelim);

        while (m_nOpPos < size() -1 && (uCh = this->operator[](m_nOpPos)) && nReadedCount < n - 1)
        {
            pCompareDelim = szdelim;
            nComparePos = m_nOpPos;

            while (*pCompareDelim && this->operator[](nComparePos) == *pCompareDelim) 
            {
                ++ nComparePos;
                ++ pCompareDelim;
            }

            if (*pCompareDelim) 
            {
                *pCurPos = uCh;
            }
            else 
            {
                m_nOpPos += nDelimLen;
                break;
            }

            ++ pCurPos;
            ++ nReadedCount;
            ++ m_nOpPos;
        }
        *pCurPos = '\0';

        return *this;
    }

    mem_file_wrapper& getline(std::string& str)
    {
        str.clear();
        unsigned char uCh = 0;
        while (m_nOpPos < size() && (uCh = this->operator[](m_nOpPos)) )
        {
            if (uCh == '\r' && this->operator[](m_nOpPos + 1) == '\n') 
            {
                m_nOpPos += 2;
                break;
            }
            else if (uCh == '\n') 
            {
                ++ m_nOpPos;
                break;
            }
            else 
            {
                str.push_back(uCh);
            }

            ++ m_nOpPos;
        }

        return *this;
    }

    int get()
    {
        return this->operator[](m_nOpPos ++);
    }

    mem_file_wrapper& get (char& c)
    {
        c = get();
        return *this;
    }

    mem_file_wrapper& get (char* s, streamsize n)
    {
        _ASSERT(s);

        size_t nReadedCount = 0;
        auto pCurPos = s;

        while (m_nOpPos < size() && nReadedCount < n - 1)
        {
            *pCurPos = this->operator[](m_nOpPos);

            ++ pCurPos;
            ++ nReadedCount;
            ++ m_nOpPos;
        }
        *pCurPos = '\0';

        return *this;
    }

    mem_file_wrapper& get (char* s, streamsize n, char delim)
    {
        _ASSERT(s);

        size_t nReadedCount = 0;
        auto pCurPos = s;
        unsigned char uCh = 0;
        while (m_nOpPos < size() && nReadedCount < n - 1)
        {
            uCh = this->operator[](m_nOpPos);
            if (uCh != delim) 
            {
                *pCurPos = uCh;
            }
            else 
            {
                ++ m_nOpPos;
                break;
            }

            ++ pCurPos;
            ++ nReadedCount;
            ++ m_nOpPos;
        }
        *pCurPos = '\0';

        return *this;
    }

    bool eof()
    {
        _ASSERT(m_nOpPos <= size());
        return m_nOpPos >= size();
    }

    operator bool()
    {
        return ! eof();
    }


protected:
    size_t   m_nOpPos;              ///< 操作中的位置
};


}}
