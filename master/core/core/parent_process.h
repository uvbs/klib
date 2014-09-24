#ifndef _klib_parent_process_h_
#define _klib_parent_process_h_

#include <string>

namespace klib{
namespace core{


/* get parent process name */
class parent_process
{
public:
    static std::string get_parent_process();

};


}}



#endif // _klib_parent_process_h_