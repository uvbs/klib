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

std::string code_convert::utf8_2_gbk(const std::string& strUtf8Code)  
{  
    std::string strRet;
    std::wstring wstr = utf8_2_unicode(strUtf8Code);
    return unicode_2_gbk(wstr);
}

std::wstring code_convert::gbk_2_unicode(const std::string& strascii)  
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

std::string code_convert::gbk_2_utf8(const std::string& strAsciiCode)  
{
    std::string strRet;
    std::wstring wstr = gbk_2_unicode(strAsciiCode);
    strRet = unicode_2_utf8(wstr);
    return std::move(strRet);
}  

bool code_convert::is_gbk(const char* gb, int length)
{
    for(int i=0; i<length; ++i){
        if(gb[i]<0)
        {
            return true;
        }
    }
    return false;
}

bool code_convert::is_utf8(const char* str, int length)
{
    int i;
    long nBytes=0;//UFT8可用1-6个字节编码,ASCII用一个字节
    unsigned char chr;
    bool bAllAscii=true; //如果全部都是ASCII, 说明不是UTF-8
    for(i=0;i<length;i++)
    {
        chr= *(str+i);
        if( (chr&0x80) != 0 ) // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
            bAllAscii= false;
        if(nBytes==0) //如果不是ASCII码,应该是多字节符,计算字节数
        {
            if(chr>=0x80)
            {
                if(chr>=0xFC&&chr<=0xFD)
                    nBytes=6;
                else if(chr>=0xF8)
                    nBytes=5;
                else if(chr>=0xF0)
                    nBytes=4;
                else if(chr>=0xE0)
                    nBytes=3;
                else if(chr>=0xC0)
                    nBytes=2;
                else
                {
                    return false;
                }
                nBytes--;
            }
        }
        else //多字节符的非首字节,应为 10xxxxxx
        {
            if( (chr&0xC0) != 0x80 )
            {
                return false;
            }
            nBytes--;
        }
    }
    if( nBytes > 0 ) //违返规则
    {
        return false;
    }
    if( bAllAscii ) //如果全部都是ASCII, 说明不是UTF-8
    {
        return false;
    }
    return true;
}


}}