
#include "ensure.h"
#include <windows.h>
#include <vector>


using namespace klib::debug;



ensure::~ensure()  
{
    if (op_type_ & ENSURE_DEBUG_ALERT) 
    {
        _ASSERT_EXPR_ENSURE(FALSE, this->what().c_str());
    }

    if (op_type_ & ENSURE_DEBUG_EXCEPTION) 
    {
        throw std::exception(this->what().c_str());
    }
}

ensure& ensure::set_context(ensure_op_type t, char const* expr, char const* file, int line)
{
    err_.str("");
    err_ << "Ensure failed, expression: \"" << expr << "\", values: " << std::endl;
    op_type_ = t;
    file_    = file;
    line_    = line;

    // 这里你可以将std::cerr替换为你自己项目的日志系统
    err_ <<
        "Ensure failed point: [" << file_ << ":" << line_ << "]" << std::endl;
    return *this;
}
    
ensure& ensure::set_current_val(char src, char const* name)
{
    int tmp = (int)src;
    return set_val(tmp, name);
}
ensure& ensure::set_current_val(bool src, char const* name)
{
    return set_val(src, name);
}
ensure& ensure::set_current_val(short src, char const* name)
{
    return set_val(src, name);
}
ensure& ensure::set_current_val(int src, char const* name)
{
    return set_val(src, name);
}
ensure& ensure::set_current_val(std::string const& src, char const* name)
{
    err_ << "\t" << name << " = " << src << ", size: " << src.size() << "; " << std::endl;
    return *this;
}
ensure& ensure::set_current_val(char const* src, char const* name)
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
ensure& ensure::set_current_val(char * src, char const* name)
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
ensure& ensure::set_current_val(const wchar_t * src, char const* name)
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
ensure& ensure::set_current_val( wchar_t * src, char const* name)
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
    
    
/// 抛出异常
ensure& ensure::set_current_val(std::exception const& ex, char const*)
{
    std::string str_err = err_.str() + "\t";
    str_err += ex.what();
    std::exception local(str_err.c_str());
    throw local;

    return *this;
}

std::string ensure::what()
{
    return err_.str();
}

#ifdef _WIN32

///< 转换为ascii编码
std::string ensure::wide_2_gbk(const std::wstring& wstr_code)  
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
