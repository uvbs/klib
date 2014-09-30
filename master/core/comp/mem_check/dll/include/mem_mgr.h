#ifndef _klib_mem_mgr_h_
#define _klib_mem_mgr_h_



#include "../include/allocator.h"
#include "../include/lock.h"
#include "addr_mgr.h"
#include "view_define.h"

#include <vector>


// memory mgr
class mem_mgr
{
public:
    mem_mgr();
    static mem_mgr* instance();

public:
    addr_mgr* create();
    bool free(addr_mgr* cur, void* ptr);
    void enable_stats(bool enable);

    simp_string debug_output(const char* desc);

    simp_string stats(const char* desc);
    bool write_file(char* desc, const char* filename);
    bool for_each(const view_addr_info_func& func);
    
protected:
    critical_section                                m_addr_cs;
    std::vector<addr_mgr*, MemAlloc<addr_mgr*> >    m_mgr_arr;

    bool                                            m_enable_stats;
    static mem_mgr*                                 m_instance;
};




#endif //