#ifndef _klib_fast_string_h
#define _klib_fast_string_h


#include <string>
using std::string;

namespace klib{
namespace core {


inline size_t GetLen(const char * s){return strlen(s);}
inline void Append(string & r, const char * s, size_t len){r.append(s, len);}

inline size_t GetLen(const string & s){return s.length();}
inline void Append(string & r, const string & s, size_t len){r.append(s.data(), len);}

//如要支持CString AnsiString，为它们添加GetLen、Append函数即可

class NullType{};

template<typename T1, typename T2 = NullType>
struct fast_string
{
    fast_string(const T1 & s1, const T2 & s2);
    operator string() const;
    inline size_t length() const {return s1_len + s2_len;}
public:
    const T1 &      s1_;
    size_t          s1_len;
    const T2 &      s2_;
    size_t          s2_len;
};

template<typename T1>
struct fast_string<T1, NullType>
{
    fast_string(const T1 & s1);
//    operator string() const;
    inline size_t length() const {return s1_len;}
public:
    const T1 &      s1_;
    size_t          s1_len;
};

template<typename T1, typename T2>
inline size_t GetLen(const fast_string<T1, T2> & s)
{
    return s.length();
}

template<typename T1, typename T2>
inline void Append(string & r, const fast_string<T1, T2> & s, size_t)
{
    Append(r, s.s1_, s.s1_len);
    Append(r, s.s2_, s.s2_len);
}

template<typename T1>
inline void Append(string & r, const fast_string<T1, NullType> & s, size_t)
{
    Append(r, s.s1_, s.s1_len);
}

template<typename T1, typename T2>
fast_string<T1, T2>::fast_string(const T1 & s1, const T2 & s2) : s1_(s1), s1_len(GetLen(s1)), s2_(s2), s2_len(GetLen(s2)){}
    
template<typename T1, typename T2>
fast_string<T1, T2>::operator string() const
{
    string s;
    s.reserve(length());
    Append(s, s1_, s1_len);
    Append(s, s2_, s2_len);
    return std::move(s);
}

template<typename T1>
fast_string<T1, NullType>::fast_string(const T1 & s1) : s1_(s1), s1_len(GetLen(s1)){}

/*    
template<typename T1>
    fast_string<T1, NullType>::operator string() const
{
    string s;
    s.reserve(length());
    Append(s, s1_, s1_len);
    return s;
}
*/

template<typename T1, typename T2, typename T3>
inline fast_string<fast_string<T1, T2>, T3> operator+(const fast_string<T1, T2> & x, const T3 & y)
{
    return fast_string<fast_string<T1, T2>, T3>(x, y);
}

template<typename T>    
fast_string<T> MakeFastString(const T & s)
{
    return fast_string<T>(s);                 
}



}}



#endif
