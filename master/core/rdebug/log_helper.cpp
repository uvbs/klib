
#include "log_helper.h"
#include <windows.h>
#include <vector>

#include "logger.h"

using namespace klib::debug;


#define  default_null_ptr_str ("[:nullptr]")

log_helper::~log_helper()  
{
    if (op_type_ & ENSURE_DEBUG_EXCEPTION) 
    {
        throw std::exception(this->output().c_str());
    }

    if (logger_) 
    {
        std::string str = err_.str();
        logger_->write_log_a(LOG_LEVEL_ERROR, str.c_str());
    }
}

log_helper& log_helper::set_ctx(ensure_op_type t, char const* expr, char const* file, int line)
{
    err_.str("");
    err_ << "Evaluate failed, expression: \"" << expr << "\", values: " << std::endl;
    op_type_ = t;
    file_    = file;
    line_    = line;

    // 这里你可以将std::cerr替换为你自己项目的日志系统
    err_ <<
        "Evaluate failed point: [" << file_ << ":" << line_ << "]" << std::endl;
    return *this;
}
    
log_helper& log_helper::serilize(char src, char const* name)
{
    int tmp = (int)src;
    return serilize_tpl(tmp, name);
}
log_helper& log_helper::serilize(const bin_buf& buf, const char* name)
{
    if (nullptr == buf .ptr_) {
        err_ << default_null_ptr_str;
    }
    else {
        err_ << std::string(buf.ptr_, buf.size_);
    }
    return *this;
}
log_helper& log_helper::serilize(const str_buf& buf, const char* name)
{
    if (nullptr == buf .ptr_) {
        err_ << default_null_ptr_str;
    }
    else {
        err_ << std::string(buf.ptr_, buf.size_);
    }
    return *this;
}
log_helper& log_helper::serilize(bool src, char const* name)
{
    return serilize_tpl(src, name);
}
log_helper& log_helper::serilize(short src, char const* name)
{
    return serilize_tpl(src, name);
}
log_helper& log_helper::serilize(int src, char const* name)
{
    return serilize_tpl(src, name);
}
log_helper& log_helper::serilize(std::string const& src, char const* name)
{
    err_ << "\t" << name << " = " << src << ", size: " << src.size() << "; " << std::endl;
    return *this;
}
log_helper& log_helper::serilize(char const* src, char const* name)
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
log_helper& log_helper::serilize(char * src, char const* name)
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
log_helper& log_helper::serilize(const wchar_t * src, char const* name)
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
log_helper& log_helper::serilize( wchar_t * src, char const* name)
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
    

//----------------------------------------------------------------------
//

log_helper& log_helper::operator << (char src)
{
    serilize_tpl(src);
    return *this;
}

log_helper& log_helper::operator << (bool src)
{
    serilize_tpl(src);
    return *this;
}

log_helper& log_helper::operator << (short src)
{
    serilize_tpl(src);
    return *this;
}

log_helper& log_helper::operator << (int src)
{
    serilize_tpl(src);
    return *this;
}

log_helper& log_helper::operator << (std::string const& src)
{
    serilize_tpl(src);
    return *this;
}

log_helper& log_helper::operator << (char const* src)
{
    if (src)
    {
        err_ << src ;
    }
    else 
    {
        err_ << default_null_ptr_str ;
    }
    return *this;
}

log_helper& log_helper::operator << (char * src)
{
    if (src)
    {
        err_ << src ;
    }
    else 
    {
        err_ << default_null_ptr_str ;
    }
    return *this;
}

log_helper& log_helper::operator << (const bin_buf& buf)
{
    if (nullptr == buf .ptr_) {
        err_ << default_null_ptr_str;
    }
    else {
        err_ << std::string(buf.ptr_, buf.size_);
    }
    return *this;
}
log_helper& log_helper::operator << (const str_buf& buf)
{
    if (nullptr == buf .ptr_) {
        err_ << default_null_ptr_str;
    }
    else {
        err_ << std::string(buf.ptr_, buf.size_);
    }
    return *this;
}

#ifdef _WIN32
log_helper& log_helper::operator << (const wchar_t * src)
{
    if (src)
    {
        serilize_tpl(wide_2_gbk(src));
    }
    else 
    {
        err_ << default_null_ptr_str;
    }
    return *this;
}

log_helper& log_helper::operator << ( wchar_t * src)
{
    if (src)
    {
        serilize_tpl(wide_2_gbk(src));
    }
    else 
    {
        err_ << default_null_ptr_str;
    }
    return *this;
}

#endif



//----------------------------------------------------------------------
//

std::string log_helper::output() const
{
    return err_.str();
}

log_helper:: operator std::string() const
{
    return output();
}

#ifdef _WIN32

///< 转换为ascii编码
std::string log_helper::wide_2_gbk(const std::wstring& wstr_code)  
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
