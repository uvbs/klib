#ifndef _redefine_new_h_
#define _redefine_new_h_


#include <string>


#define GC_NEW new(__FILE__, __LINE__)  
#define new GC_NEW  


#endif  // _redefine_new_h_