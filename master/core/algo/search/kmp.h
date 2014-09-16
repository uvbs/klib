#ifndef _klib_kmp_h
#define _klib_kmp_h

#include <string>
#include <vector>

namespace klib{
namespace algo{


// 
// Kmp查找算法的实现
// TString 被查找的字符串类型 string|wstring
// max_search_len 被搜索字符串最大长度
// 
template<typename TString, int max_search_len = 300>
class kmp
{
    typedef typename TString::traits_type::char_type char_type;
    typedef std::size_t  size_type;

public:
    kmp() : m_next_len(0), m_src(nullptr), m_src_len(0) {}

    // 设置要搜索的字符串
    void set_str(const char_type* str, size_type nLen)
    {
        m_src = str;
        m_src_len = nLen;
        m_next_len = nLen;
        if (nLen > max_search_len) 
        {
            return;
        }

        next(str, nLen);
    }

    // str  在str中搜索字符串
    // str_len  str的长度
    int search_in(const char_type* str, size_type str_len)
    {
        int pos = -1;
        if (0 == m_src_len) 
            return pos;

        if (m_next_len > max_search_len) 
        {
            return -1;
        }

        int tp = 0;
        int mp = 0;  // text pointer and match string pointer;

        for(tp=0; tp < (int) str_len; ++tp)
        {  
            while(str[tp] != m_src[mp] && mp) 
            {
                mp = m_next[mp-1];
            }

            if(str[tp] == m_src[mp]) 
                ++mp;

            if(mp == m_src_len)
            {
                pos = tp - mp + 1;
                return pos;
            }
        }

        if(tp == str_len)
            pos = -1;

        return pos;
    }

    // 一把全收索了，效率略低些
    int search_once(const TString& text, const TString& src)
    {
        int pos = -1;
        if (src.empty()) 
        {
            return pos;
        }

        if (src.size() > max_search_len) 
        {
            return pos;
        }

        next(src.c_str(), src.size());

        int tp=0;
        int mp=0;  // text pointer and match string pointer;

        for(tp=0; tp < (int)text.size(); ++tp)
        {  
            while(text[tp] != src[mp] && mp) 
            {
                mp = m_next[mp-1];
            }

            if(text[tp] == src[mp]) 
                ++mp;

            if(mp == src.size())
            {  
                pos = tp - mp + 1;
                return pos;
            }
        }
        
        if(tp == text.size())
            pos = -1;

        return pos;
    }

protected:
    void next(const char_type* str, size_type nLen)  // count the longest prefex string ;
    {
        //m_next.resize(nLen);
        m_next_len = nLen;

        // the initialization of the next[0];
        m_next[0] = 0;  

        // the key iterator
        int temp;

        for(int i=1; i < (int) m_next_len; ++i)
        {     
            temp = m_next[i-1];

            while(str[i] != str[temp] && temp > 0)
            {
                temp = m_next[temp-1];
            }

            if(str[i] == str[temp]) 
            {
                m_next[i]=temp+1;
            }
            else 
            { 
                m_next[i]=0;
            }            
        }
    }

    

protected:
    // next 数组
    int m_next[max_search_len];
    size_type  m_next_len;

    // 被搜索的字符串
    const char_type* m_src;
    size_type m_src_len;
};


}}


#endif
