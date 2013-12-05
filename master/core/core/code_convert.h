#ifndef __Klib_CCONVERT__H_
#define __Klib_CCONVERT__H_


#include <vector>
#include <string>

namespace klib{
namespace encode{


//----------------------------------------------------------------------
// ±àÂë×ª»»Àà
class code_convert
{
public:
    code_convert(void);
    ~code_convert(void);

    static std::wstring utf8_2_unicode(const std::string& utf8string);  
    static std::string utf8_2_gbk(std::string& strUtf8Code); 
    static std::string unicode_2_utf8(const std::wstring& widestring); 

    static std::string unicode_2_gbk(const std::wstring& wstrcode); 
    static std::wstring gbk_2_unicode(std::string& strascii);  
    static std::string gbk_2_utf8(std::string& strAsciiCode);  
};



}}


#endif
