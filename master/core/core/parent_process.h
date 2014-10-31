#ifndef _klib_parent_process_h_
#define _klib_parent_process_h_

#include <string>
#include <windows.h>

namespace klib{
namespace core{



/* get parent process name */
class parent_process
{
public:
    static std::string get_parent_process();
    static DWORD       get_parent_process_id();
};


}}



#endif // _klib_parent_process_h_