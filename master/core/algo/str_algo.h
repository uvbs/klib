#ifndef _klib_str_algo_h
#define _klib_str_algo_h

#include <string>

#include "kmp.h"

namespace klib{
namespace algo{


typedef  std::size_t size_type;


template<typename char_type>
size_type std_find(const char_type* str, 
    size_type str_len, 
    const char_type* src,
    size_type src_len)
{
    if (0 == src_len)
    {
        return 0;
    }
    else if (src_len  > str_len)
    {
        return std::string::npos;
    }

    const char_type* cur_pos = str;
    const char_type* end_pos = str + str_len - src_len;
    for (; cur_pos < end_pos; ++cur_pos)
    {
        if (0 == std::char_traits<char_type>::compare(cur_pos, src, src_len))
        {
            return (size_type) (cur_pos - str);
        }
    }

    return std::string::npos;
}


template<typename char_type>
size_type find(const char_type* str, 
                size_type str_len, 
                const char_type* src,
                size_type src_len)
{
    static_assert(std::is_pod<char_type>::value, "must be pod!!!");

    kmp<std::basic_string<char_type, char_traits<char_type>, std::allocator<char_type>>, 1024> kmp_;

    int pos;
    kmp_.set_str(src, src_len);
    pos = kmp_.search_in(str, str_len);
    if (-1 == pos) 
    {
        return std::string::npos;
    }

    #ifdef _DEBUG
    _ASSERT(pos == kmp_.search_once(str, src));
    #endif
    
    return (size_type) pos;
}

template<typename T, typename char_type>
size_type find(const T& t, const char_type* str, size_type nLen)
{
    static_assert(std::is_class<T>::value, "must be class!!!");

    return find(t.c_str(), t.size(), str, nLen);
}

template<typename T>
size_type find(const T& t, const T& str)
{
    static_assert(std::is_class<T>::value, "must be class!!!");

    return find(t.c_str(), t.size(), str.c_str(), str.size());
}



// 快速查找帮助类
template<typename TString>
class quick_search
{
    typedef typename TString::traits_type::char_type char_type;
    typedef std::size_t  size_type;

public:
    quick_search() : m_str(nullptr), m_len(0) {}

    void set_str(const char_type* str, size_type len)
    {
        m_kmp.set_str(str, len);

        m_str = str;
        m_len = len;
    }

    size_type search_in(const char* src, size_type len)
    {
        if (m_len < 4) 
        {
            return std_find(src, len, m_str, m_len);
        }

        int pos = m_kmp.search_in(src, len);
        if (-1 == pos)
        {
            return std::string::npos;
        }
        return (size_type) pos;
    }

    // 只是一次性搜索
    size_type search_once(const char_type* str, 
        size_type str_len, 
        const char_type* src,
        size_type src_len)
    {
        static_assert(std::is_pod<char_type>::value, "must be pod!!!");

        int pos;
        m_kmp.set_str(src, src_len);
        pos = m_kmp.search_in(str, str_len);
        if (-1 == pos) 
        {
            return std::string::npos;
        }

        return (size_type) pos;
    }

    size_type search_once(const TString& str, const TString& src)
    {
        return search_once(str.c_str(), str.size(), src.c_str(), src.size());
    }

    size_type search_once(const TString& str, const char_type* src, size_type len)
    {
        return search_once(str.c_str(), str.size(), src, len);
    }
    
protected:
    kmp<TString, 1024> m_kmp;

    const char_type* m_str;
    size_type  m_len;
};


typedef quick_search<std::string>  string_seacher;
typedef quick_search<std::wstring> wstring_searcher;





}}



#endif
