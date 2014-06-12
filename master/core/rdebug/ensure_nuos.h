
//
// ensure.hpp
//
// Copyright (c) Nous Xiong.
//
// Macro ENSURE impl.
//

#ifndef _klib_ensure_nous_h
#define _klib_ensure_nous_h


#include <stdexcept>
#include <sstream>
#include <iostream>

namespace klib{
namespace debug{


class ensure
{
public:
    ensure()
        : ENSURE_A(*this)
        , ENSURE_B(*this)
        , file_(0)
        , line_(-1)
        , msg_(0)
    {  }

    ~ensure()  { }

    ensure& ENSURE_A;
    ensure& ENSURE_B;

public:
    ensure& set_context(char const* expr, char const* file, int line)
    {
        msg_ = 0;
        err_.str("");
        err_ << "Ensure failed, expression: \"" << expr << "\", values: " << std::endl;
        file_ = file;
        line_ = line;

        // 这里你可以将std::cerr替换为你自己项目的日志系统
        std::cerr <<
            "Ensure failed point: [" << file_ << "]=[" << line_ << "]" << std::endl;
        return *this;
    }

    ensure& set_current_val(char src, char const* name)
    {
        int tmp = (int)src;
        return set_val(tmp, name);
    }
    ensure& set_current_val(bool src, char const* name)
    {
        return set_val(src, name);
    }
    ensure& set_current_val(short src, char const* name)
    {
        return set_val(src, name);
    }
    ensure& set_current_val(int src, char const* name)
    {
        return set_val(src, name);
    }
    ensure& set_current_val(std::string const& src, char const* name)
    {
        err_ << "\t" << name << " = " << src << ", size: " << src.size() << "; " << std::endl;
        return *this;
    }
    ensure& set_current_val(char const* src, char const* name)
    {
        if (src)
        {
            err_ << "\t" << name << " = " << src << "; " << std::endl;
        }
        else 
        {
            err_ << "\t" << name << " = " << "nullptr" << "; " << std::endl;
        }
        return *this;
    }
    ensure& set_current_val(char * src, char const* name)
    {
        if (src)
        {
            err_ << "\t" << name << " = " << src << "; " << std::endl;
        }
        else 
        {
            err_ << "\t" << name << " = " << "nullptr" << "; " << std::endl;
        }
        return *this;
    }

#ifdef _WIN32
    ensure& set_current_val(const wchar_t * src, char const* name)
    {
        if (src)
        {
            err_ << "\t" << name << " = " << wide_2_gbk(src) << "; " << std::endl;
        }
        else 
        {
            err_ << "\t" << name << " = " << "nullptr" << "; " << std::endl;
        }
        return *this;
    }
    ensure& set_current_val( wchar_t * src, char const* name)
    {
        if (src)
        {
            err_ << "\t" << name << " = " << wide_2_gbk(src) << "; " << std::endl;
        }
        else 
        {
            err_ << "\t" << name << " = " << "nullptr" << "; " << std::endl;
        }
        return *this;
    }
#endif
    
    template<typename _type>
    ensure& set_current_val(_type const src, char const* name)
    {
        if (std::is_pointer<_type>::value)
        {
            err_ << "\t" << name << " = " << src << "; val is a pointer" << std::endl;
        }
        else
        {
            err_ << "\t" << name << " = " << src << "; " << std::endl;
        }
        return *this;
    }

    // 你可以加入任何你项目需要的数据类型
    // 例如：
    //ensure& set_current_val(my_data const& src, char const* name)
    //{
    //  err_ << name << " = " << src.get_xxx() << "; ";
    //  return *this;
    //}

    /// 用户可添加的消息
    /*
    ensure& set_current_val(char const* msg, char const*)
    {
        msg_ = msg;
        return *this;
    }
    */

    /// 抛出异常
    ensure& set_current_val(std::exception const& ex, char const*)
    {
        if (msg_)
        {
            err_ << "\t" << "user msg = " << msg_ << std::endl;
        }

        std::string str_err = err_.str() + "\t";
        str_err += ex.what();
        std::exception local(str_err.c_str());
        throw local;

        return *this;
    }

    static ensure get_ensure()
    {
        // 仅仅单线程
        //static ensure ret;
        //return ret;
        return ensure();
    }

private:
    template <typename T>
    ensure& set_val(T const t, char const* name)
    {
        err_ << "\t" << name << " = " << t << "; " << std::endl;
        return *this;
    }

#ifdef _WIN32
    ///< 转换为ascii编码
    std::string wide_2_gbk(const std::wstring& wstr_code)  
    {  
        int nAsciiSize = ::WideCharToMultiByte(CP_OEMCP, 0, wstr_code.c_str(), -1, NULL, 0, NULL, NULL);  
        if (nAsciiSize == ERROR_NO_UNICODE_TRANSLATION) 
        {
            return std::string();
        }
        if (nAsciiSize == 0)
        {
            return std::string();
        }
        std::vector<char> resultstring(nAsciiSize);
        int nConvResult = ::WideCharToMultiByte(CP_OEMCP, 0, wstr_code.c_str(), -1, &resultstring[0], nAsciiSize, NULL, NULL);  

        if (nConvResult != nAsciiSize) 
        {
            return std::string();
        }  

        return std::string(&resultstring[0]);  
    }  
#endif

private:
    std::stringstream err_;
    char const* file_;
    int line_;
    char const* msg_;
};

}}


#define ENSURE_A(x) ENSURE_OP(x, B)
#define ENSURE_B(x) ENSURE_OP(x, A)

#define ENSURE_OP(x, next) \
    ENSURE_A.set_current_val((x), #x).ENSURE_##next

#define ENSURE(expr) \
    if( (expr) ) ; \
  else klib::debug::ensure::get_ensure().set_context(#expr,__FILE__,__LINE__).ENSURE_A



#endif /* _klib_ensure_nous_h */