#include "code_convert.h"

#include<windows.h>

namespace klib {
namespace encode {



code_convert::code_convert(void)
{
}

code_convert::~code_convert(void)
{
}

std::wstring code_convert::utf8_2_unicode(const std::string& utf8string)  
{  
    int widesize = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, NULL, 0);  
    if (widesize == 0)  
    {
		if (ERROR_NO_UNICODE_TRANSLATION == GetLastError())
			return std::wstring();
    }

    if (widesize == 0)
    {  
        return std::wstring();
    }  

    std::vector<wchar_t> resultstring(widesize);
    int convresult = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &resultstring[0], widesize);  

    if (convresult != widesize)
    {  
        return std::wstring();
    }

    return std::wstring(&resultstring[0]);  
}  

std::string code_convert::unicode_2_gbk(const std::wstring& wstrcode)  
{  
    int asciisize = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, NULL, 0, NULL, NULL);  
    if (0 == asciisize)  
    {
		if (ERROR_NO_UNICODE_TRANSLATION == GetLastError())
			return std::string();
    }  

    if (asciisize == 0)  
    {  
        return std::string();
    }  

    std::vector<char> resultstring(asciisize);  
    int convresult =::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, &resultstring[0], asciisize, NULL, NULL);  

    if (convresult != asciisize)  
    {  
        return std::string();
    }  

    return std::string(&resultstring[0]);  
}  

std::string code_convert::utf8_2_gbk(std::string& strUtf8Code)  
{  
    std::string strRet;
    std::wstring wstr = utf8_2_unicode(strUtf8Code);
    return unicode_2_gbk(wstr);
}

std::wstring code_convert::gbk_2_unicode(std::string& strascii)  
{  
    int widesize = MultiByteToWideChar (CP_ACP, 0, (char*)strascii.c_str(), -1, NULL, 0);  
    if (0 == widesize)  
    {
		if (ERROR_NO_UNICODE_TRANSLATION == GetLastError())
			return std::wstring();
    }  
    if (widesize == 0)  
    {  
        return std::wstring();
    }  
    std::vector<wchar_t> resultstring(widesize);  
    int convresult = MultiByteToWideChar (CP_ACP, 0, (char*)strascii.c_str(), -1, &resultstring[0], widesize);  

    if (convresult != widesize)  
    {  
        return std::wstring();
    }  

    return std::wstring(&resultstring[0]);  
}

std::string code_convert::unicode_2_utf8(const std::wstring& widestring)  
{  
    int utf8size = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, NULL, 0, NULL, NULL);
    if (utf8size == 0)
    {
        return std::string();
    }

    std::vector<char> resultstring(utf8size);

    int convresult = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, &resultstring[0], utf8size, NULL, NULL);

    if (convresult != utf8size)
    {
        return std::string();
    }

    return std::string(&resultstring[0]);  
}

std::string code_convert::gbk_2_utf8(std::string& strAsciiCode)  
{
    std::string strRet;
    std::wstring wstr = gbk_2_unicode(strAsciiCode);
    strRet = unicode_2_utf8(wstr);
    return std::move(strRet);
}  


}}