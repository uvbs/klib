#ifndef _klib_system_info_h_
#define _klib_system_info_h_


#include <string>


namespace klib{
namespace sys {

class system_info
{
public:
    // 获取操作系统位数
    int get_system_bits() ;

    //读取操作系统的名称  
    void get_system_name(std::string& osname);

    //读取操作系统的版本名称  
    void get_version_mark(std::string& vmark);

protected:

};


}}




#endif // _klib_system_info_h_