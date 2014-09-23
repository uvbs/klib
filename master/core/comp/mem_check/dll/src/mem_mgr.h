#ifndef _klib_mem_mgr_h_
#define _klib_mem_mgr_h_



#include "../include/allocator.h"
#include "addr_mgr.h"

#include <vector>


// memory mgr
class mem_mgr
{
public:
    addr_mgr* create();

    simp_string stats();

protected:
    std::vector<addr_mgr*> mgr_arr_;

};




#endif //