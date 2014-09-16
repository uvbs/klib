#ifndef _klib_find_tool_h
#define _klib_find_tool_h

#include <crtdbg.h>

#include <iostream>


namespace klib{
namespace core {


/**
 * @brief 查找工具类
 */
template<typename T>
class find_tool
{
public:
    enum emCaseSenseType
    {
        emCaseSensitve,             ///< 大小写敏感
        emCaseInsensitive           ///< 大小写不敏感
    };

public:
    find_tool(const T* str) : m_pt(str) 
    {
        _ASSERT(str);
        if (sizeof(T) == sizeof(wchar_t))
        {
            m_nBuffLen = wcslen((wchar_t*) str);
        }
        else
        {
            m_nBuffLen = strlen((char*) str);
        }
    }
    find_tool(const T* str, size_t nLen) : m_pt(str) , m_nBuffLen(nLen)
    { }

private:
    find_tool(find_tool&);

public:
    __forceinline find_tool* operator -> () 
    {
        return this;
    }

    ///< 定位
    const T* beginptr();
    const T* endptr(size_t nItem);

    ///< 查找
    const T* find(const T* pszStr, emCaseSenseType type = emCaseSensitve);
    const T* find(const T* pszStr, size_t nLen, emCaseSenseType type = emCaseSensitve);
    static const T* find(const T* pszBuff, const T* pszStr, emCaseSenseType type = emCaseSensitve);
    static const T* find(const T* pszBuff, size_t nBuffLen, const T* pszStr, size_t nLen, emCaseSenseType type = emCaseSensitve);

    ///< 以什么开始
    bool beginwith(const T* pszStr);
    bool beginwith(const T* pszStr, size_t nLen);
    static bool beginwith(const T* pszBuff, const T* pszStr);
    static bool beginwith(const T* pszBuff, const T* pszStr, size_t nLen);

    ///< 以什么结束
    bool endwith(const T* pszStr);
    bool endwith(const T* pszStr, size_t nLen);
    static bool endwith(const T* pszBuff, const T* pszStr);
    static bool endwith(const T* pszBuff, size_t nBuffLen, const T* pszStr, size_t nLen);

protected:
    const T*    m_pt;               ///< 缓冲区指针
    size_t      m_nBuffLen;         ///< 缓冲区的长度
};

typedef  find_tool<wchar_t> WideFindTool;
typedef  find_tool<char>    MutiFindTool;


//------------------------------------------------------------------------
// implement
template<typename T>
const T* find_tool<T>::beginptr()
{
    return m_pt;
}

template<typename T>
const T* find_tool<T>::endptr(size_t nItem)
{
    return ((m_pt + m_nBuffLen) - nItem);
}

template<typename T>
const T* find_tool<T>::find(const T* pszStr, emCaseSenseType type)
{
    return find(m_pt, pszStr, type);
}

template<typename T>
const T* find_tool<T>::find(const T* pszStr, size_t nLen, emCaseSenseType type)
{
    return find(m_pt, m_nBuffLen, pszStr, nLen, type);
}

template<typename T>
const T* find_tool<T>::find(const T* pszBuff, const T* pszStr, emCaseSenseType type)
{
    if (NULL == pszBuff || NULL == pszStr) 
    {
        return NULL;
    }
    
    if (sizeof(T) == sizeof(wchar_t)) 
    {        
        return find(pszBuff, wcslen((wchar_t*)pszBuff), pszStr, wcslen((wchar_t*)pszStr), type);
    }
    else 
    {
        return find(pszBuff, strlen((char*)pszBuff), pszStr, strlen((char*)pszStr), type);
    }
}

template<typename T>
const T* find_tool<T>::find(const T* pszBuff, size_t nBuffLen, const T* pszStr, size_t nLen, emCaseSenseType type)
{
    if (nLen == 0) 
    {
        return NULL;
    }
    if (NULL == pszBuff || NULL == pszStr) 
    {
        return NULL;
    }

    if (nBuffLen < nLen) 
    {
        return NULL;
    }

    size_t nDiff = nBuffLen - nLen;
    const T* pEndCmpPos = pszBuff + nDiff;
    const T* pCurPos = pszBuff;

    auto wide_cmp = wcsncmp;
    auto muti_cmp = strncmp;

    if (emCaseInsensitive == type)
    {
        wide_cmp = _wcsnicmp;
        muti_cmp = _strnicmp;
    }

    // 比较数据
    if (sizeof(T) == sizeof(wchar_t)) 
    {
        for (; pCurPos <= pEndCmpPos; ++ pCurPos) 
        {
            if (wide_cmp((wchar_t*)pCurPos, (wchar_t*) pszStr, nLen) == 0)
            {
                return pCurPos;
            }
        }
    }
    else
    {
        for (; pCurPos <= pEndCmpPos; ++ pCurPos) 
        {
            if (muti_cmp((char*)pCurPos, (char*)pszStr, nLen) == 0)
            {
                return pCurPos;
            }
        }
    }

    return NULL;
}

template<typename T>
bool find_tool<T>::beginwith(const T* pszStr)
{
    return beginwith(m_pt, pszStr);
}

template<typename T>
bool find_tool<T>::beginwith(const T* pszStr, size_t nLen)
{
    return beginwith(m_pt, pszStr, nLen);
}

template<typename T>
bool find_tool<T>::beginwith(const T* pszBuff, const T* pszStr)
{
    if (sizeof(T) == sizeof(wchar_t)) 
    {
        return beginwith(pszBuff, pszStr, wcslen((wchar_t*)pszStr));
    }
    else if (sizeof(T) == sizeof(char))
    {
        return beginwith(pszBuff, pszStr, strlen((char*)pszStr));
    }

    _ASSERT(false);
    return true;
}

template<typename T>
bool find_tool<T>::beginwith(const T* pszBuff, const T* pszStr, size_t nLen)
{
    if (nLen <= 0) 
    {
        return false;
    }

    if (sizeof(T) == sizeof(wchar_t)) 
    {
        nLen <<= 1;
        if (memcmp(pszBuff, pszStr, nLen) == 0) 
        {
            return true;
        }
        return false;
    }
    else if (sizeof(T) == sizeof(char))
    {
        if (memcmp(pszBuff, pszStr, nLen) == 0) 
        {
            return true;
        }
        return false;
    }

    _ASSERT(false);
    return true;
}

template<typename T>
bool find_tool<T>::endwith(const T* pszStr)
{
    return endwith(m_pt, pszStr);
}

template<typename T>
bool find_tool<T>::endwith(const T* pszStr, size_t nLen)
{
    return endwith(m_pt, m_nBuffLen, pszStr, nLen);
}

template<typename T>
bool find_tool<T>::endwith(const T* pszBuff, const T* pszStr)
{
    size_t nSrcLen = 0;
    size_t nBuffLen = 0;
    if (sizeof(T) == sizeof(wchar_t)) 
    {
        nBuffLen = wcslen((wchar_t*)pszBuff);
        nSrcLen = wcslen((wchar_t*)pszStr);
    }
    else
    {
        nBuffLen = strlen((char*)pszBuff);
        nSrcLen = strlen((char*)pszStr);
    }

    return endwith(pszBuff, nBuffLen, pszStr, nSrcLen);
}

template<typename T>
bool find_tool<T>::endwith(const T* pszBuff, size_t nBuffLen, const T* pszStr, size_t nLen)
{
    if (nLen <= 0) 
    {
        return false;
    }

    if (nBuffLen < nLen)
    {
        return false;
    }

    if (sizeof(T) == sizeof(wchar_t)) 
    {
        nBuffLen <<= 1;
        nLen <<= 1;

        return (0 == memcmp((char*)pszBuff + nBuffLen - nLen, pszStr, nLen));
    }
    else
    {
        return (0 == memcmp((char*)pszBuff + nBuffLen - nLen, pszStr, nLen));
    }
}



}}


#endif
