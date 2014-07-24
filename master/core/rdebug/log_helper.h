#pragma once

#include <string>
#include <sstream>



#include "logger.h"


namespace klib{
namespace debug{


#define ENSURE_DEBUG_ALERT      (0x01)
#define ENSURE_DEBUG_EXCEPTION  (0x02)

typedef unsigned int ensure_op_type;

// 二进制缓冲区
struct bin_buf
{
    bin_buf(const char* buf, size_t len) : ptr_(buf), size_(len)
    {
    }
    const char* ptr_;
    size_t      size_;
};

// 定义的字符串缓冲区
struct str_buf
{
    str_buf(const char* buf, size_t len) : ptr_(buf), size_(len)
    {
    }
    const char* ptr_;
    size_t      size_;
};

// 日志帮助类
class log_helper
{
public:
    log_helper(Logger* loger = nullptr)
    : LOG_FORMATOR_A(*this)
    , LOG_FORMATOR_B(*this)
    , file_(0)
    , line_(-1)
    , op_type_(0)
    , logger_(loger)
    {  }

    ~log_helper();

    log_helper& LOG_FORMATOR_A;
    log_helper& LOG_FORMATOR_B;

public:
    log_helper& set_ctx(ensure_op_type t, 
        char const* expr, 
        char const* file, 
        int line);

    //----------------------------------------------------------------------
    // serilize
    log_helper& serilize(char src, char const* name);
    log_helper& serilize(bool src, char const* name);
    log_helper& serilize(short src, char const* name);
    log_helper& serilize(int src, char const* name);
    log_helper& serilize(std::string const& src, char const* name);
    log_helper& serilize(char const* src, char const* name);
    log_helper& serilize(char * src, char const* name);
    log_helper& serilize(const bin_buf& buf, const char* name);
    log_helper& serilize(const str_buf& buf, const char* name);
    template<typename _type>
    log_helper& serilize(_type const src, char const* name);

#ifdef _WIN32
    log_helper& serilize(const wchar_t * src, char const* name);
    log_helper& serilize( wchar_t * src, char const* name);
#endif
    
    //----------------------------------------------------------------------
    // stream api
    log_helper& operator << (char src);
    log_helper& operator << (bool src);
    log_helper& operator << (short src);
    log_helper& operator << (int src);
    log_helper& operator << (std::string const& src);
    log_helper& operator << (char const* src);
    log_helper& operator << (char * src);
    log_helper& operator << (const bin_buf& buf);
    log_helper& operator << (const str_buf& buf);
    template<typename _type>
    log_helper& operator << (_type const src);

#ifdef _WIN32
    log_helper& operator << (const wchar_t * src);
    log_helper& operator << ( wchar_t * src);
#endif

    //----------------------------------------------------------------------
    //
    std::string output() const;
    operator std::string() const;
    static log_helper get_formator() {  return log_helper(); }

private:
    template <typename T>
    log_helper& serilize_tpl(T const t, char const* name);

    template <typename T>
    log_helper& serilize_tpl(T const t);

#ifdef _WIN32
    std::string wide_2_gbk(const std::wstring& wstr_code);
#endif

    std::string format_hex(const bin_buf& buf);
    std::string format_ascii(const bin_buf& buf);
    std::string format_hex_ascii(const bin_buf& buf);

private:
    std::stringstream       err_;
    char const*             file_;
    int                     line_;
    ensure_op_type          op_type_;
    Logger*                 logger_;
};


//----------------------------------------------------------------------
// imp

template<typename _type>
log_helper& log_helper::serilize(_type const src, char const* name)
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

template <typename T>
log_helper& log_helper::serilize_tpl(T const t, char const* name)
{
    err_ << "\t" << name << " = " << t << "; " << std::endl;
    return *this;
}


template <typename T>
log_helper& log_helper::serilize_tpl(T const t)
{
    err_ << t;
    return *this;
}

template<typename _type>
log_helper& log_helper::operator << (_type const src)
{
    err_ << src << std::endl;
    return *this;
}

inline log_helper get_formator(Logger* loger = nullptr) 
{
    if (nullptr == loger)
    {
        loger = logger_mgr::instance()->default_logger();
    }

    return log_helper(loger); 
}


}}

using klib::debug::get_formator;

// 宏无限展开部分
#define LOG_FORMATOR_A(x) LOG_OP_X(x, B)
#define LOG_FORMATOR_B(x) LOG_OP_X(x, A)

#define LOG_OP_X(x, next) \
    LOG_FORMATOR_A.serilize((x), #x).LOG_FORMATOR_##next

// 条件成功的时候记录日志
#define LOG_IF(expr) \
    if( !(expr) ) ; \
  else get_formator(klib::debug::Logger::instance()).set_ctx(ENSURE_DEBUG_ALERT, \
                #expr,\
                __FILE__,\
                __LINE__).LOG_FORMATOR_A

// 同上（可以指定输出到哪个日志）
#define LOG_IF_D(loger, expr) \
    if( !(expr) ) ; \
  else get_formator(loger).set_ctx(ENSURE_DEBUG_ALERT, \
                #expr,\
                __FILE__,\
                __LINE__).LOG_FORMATOR_A





