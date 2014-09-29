#ifndef _view_define_h_
#define _view_define_h_




#include <functional>


#include "addr_mgr.h"


typedef std::function<void(const char*, const addr_stats_info&)> view_info_func;



#endif // _view_define_h_