#ifndef _klib_ensure_h
#define _klib_ensure_h

#include <exception>
#include <sstream>
#include <crtdbg.h>
#include <vector>


namespace klib {
namespace debug {



#ifdef _DEBUG

#define _ASSERT_EXPR_ENSURE(expr, msg) \
    (void) ((!!(expr)) || \
    (1 != _CrtDbgReport(_CRT_ASSERT, (__FILE__), __LINE__, NULL, msg)) || \
    (_CrtDbgBreak(), 0))

#else
#define _ASSERT_EXPR_ENSURE(expr, msg) 
#endif


/**
 * @brief 断言时显示变量的值 
 */
class EnsureAlert
{
public:
    EnsureAlert(const char *exp, const char *file, int line)
    {
        std::ostringstream so;
        so << "ensure failed : " << exp << '\n';
        so << file << '(' << line << ')' << '\n';
        so << "context variables:\n";
        m_what = so.str();
    }
    ~EnsureAlert() throw ()
    {
        _ASSERT_EXPR_ENSURE(FALSE, this->what());
    }

    ///< 处理其它模板类型
    template<typename T>
    EnsureAlert& operator << (const std::pair<const char *, T>& p)
    {
        std::ostringstream so;
        so << '\t' << p.first << " : " << p.second << '\n';
        m_what += so.str();
        return *this;
    }

    ///< 处理wstring的序列化
    EnsureAlert& operator << (const std::pair<const char *, std::wstring>& p)
    {
        std::ostringstream so;
        so << '\t' << p.first << " : " << WideByte2Acsi(p.second) << '\n';
        m_what += so.str();
        return *this;
    }

    ///< 处理wchar_t的序列化
    EnsureAlert& operator << (const std::pair<const char *, wchar_t*>& p)
    {
        if (p.second) 
        {
            std::ostringstream so;
            so << '\t' << p.first << " : " << WideByte2Acsi(p.second) << '\n';
            m_what += so.str();
        }        
        return *this;
    }

    ///< 处理其它int类型的序列化
    EnsureAlert& operator << (int){ return *this; }

    const char *what() const throw ()
    { 
        return m_what.c_str();
    }

protected:
    ///< 转换为ascii编码
    std::string WideByte2Acsi(const std::wstring& wstrcode)  
    {  
        int nAsciiSize = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, NULL, 0, NULL, NULL);  
        if (nAsciiSize == ERROR_NO_UNICODE_TRANSLATION) 
        {
            return std::string();
        }
        if (nAsciiSize == 0)
        {
            return std::string();
        }
        std::vector<char> resultstring(nAsciiSize);
        int nConvResult = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, &resultstring[0], nAsciiSize, NULL, NULL);  

        if (nConvResult != nAsciiSize) 
        {
            return std::string();
        }  

        return std::string(&resultstring[0]);  
    }  

private:
    mutable std::string m_what;
};


static int A = 0, B = 0;
#define AB(a, N) std::make_pair(#a, a) << N
#define A(a) AB(a, B)
#define B(a) AB(a, A)
#define ENSURE_ASSERT(b)  if (b); \
    else  EnsureAlert(#b, __FILE__, __LINE__) << A 


}} // namespace 



#endif
