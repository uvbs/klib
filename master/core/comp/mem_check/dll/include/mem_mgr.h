#ifndef _klib_mem_mgr_h_
#define _klib_mem_mgr_h_



#include "../include/allocator.h"
#include "addr_mgr.h"

#include <vector>


// memory mgr
class mem_mgr
{
public:
    static mem_mgr* instance();

public:
    addr_mgr* create();

    simp_string stats();

protected:
    std::vector<addr_mgr*, MemAlloc<addr_mgr*> > m_mgr_arr;

    static mem_mgr* m_instance;
};




#endif //