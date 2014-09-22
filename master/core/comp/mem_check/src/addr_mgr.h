#ifndef _klib_addr_mgr_h_
#define _klib_addr_mgr_h_

#include <map>

#include "../include/allocator.h"


struct addr_info
{
    addr_info() : nsize(0) {}

    size_t nsize;
};


// Í³¼Æ
class addr_mgr
{
public:

public:
    bool add_addr_info(void* p, size_t nsize);
    bool del_addr_info(void* p, size_t& nsize);

protected:
    std::map<void*, 
             addr_info, 
             std::less<void*>, 
             MemAlloc<std::pair<void*, addr_info>> >    m_addr_infos;
};



#endif // _klib_addr_mgr_h_