
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


#define ENSURE_DEBUG_LOG        (0x01)
#define ENSURE_DEBUG_ALERT      (0x02)
#define ENSURE_DEBUG_EXCEPTION  (0x04)

typedef unsigned int ensure_op_type;

#ifdef _DEBUG

#define _ASSERT_EXPR_ENSURE(expr, msg) \
    (void) ((!!(expr)) || \
    (1 != _CrtDbgReport(_CRT_ASSERT, (__FILE__), __LINE__, NULL, msg)) || \
    (_CrtDbgBreak(), 0))

#else
#define _ASSERT_EXPR_ENSURE(expr, msg) 
#endif

class ensure
{
public:
    ensure()
        : ENSURE_A(*this)
        , ENSURE_B(*this)
        , file_(0)
        , line_(-1)
        , op_type_(0)
    {  }

    ~ensure()  ;

    ensure& ENSURE_A;
    ensure& ENSURE_B;

public:
    ensure& set_context(ensure_op_type t, char const* expr, char const* file, int line);
    
    ensure& set_current_val(char src, char const* name);
    ensure& set_current_val(bool src, char const* name);
    ensure& set_current_val(short src, char const* name);
    ensure& set_current_val(int src, char const* name);
    ensure& set_current_val(std::string const& src, char const* name);
    ensure& set_current_val(char const* src, char const* name);
    ensure& set_current_val(char * src, char const* name);

#ifdef _WIN32
    ensure& set_current_val(const wchar_t * src, char const* name);
    ensure& set_current_val( wchar_t * src, char const* name);
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
    
    /// 抛出异常
    ensure& set_current_val(std::exception const& ex, char const*);

    std::string what();

    static ensure get_ensure()
    {
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
    std::string wide_2_gbk(const std::wstring& wstr_code)  ;
#endif

private:
    std::stringstream err_;
    char const*       file_;
    int               line_;
    ensure_op_type    op_type_;
};

}}


#define ENSURE_A(x) ENSURE_OP(x, B)
#define ENSURE_B(x) ENSURE_OP(x, A)

#define ENSURE_OP(x, next) \
    ENSURE_A.set_current_val((x), #x).ENSURE_##next

#define ENSURE_ASSERT(expr) \
    if( (expr) ) ; \
  else klib::debug::ensure().set_context(ENSURE_DEBUG_ALERT, #expr,__FILE__,__LINE__).ENSURE_A

#define ENSURE_VERIFY(expr) \
    if( (expr) ) ; \
  else klib::debug::ensure().set_context(ENSURE_DEBUG_ALERT|ENSURE_DEBUG_EXCEPTION, #expr,__FILE__,__LINE__).ENSURE_A


#endif /* _klib_ensure_nous_h */