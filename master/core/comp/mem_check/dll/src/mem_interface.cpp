#include "../include/mem_interface.h"

#include "../include/mem_mgr.h"


static mem_interface g_mem_interface;


addr_mgr* mem_interface::create_mgr()
{
    mem_mgr* mmgr = mem_mgr::instance();
    return mmgr->create();
}

bool mem_interface::global_free_addr(void* ptr)
{
    // @todo to free to another mgr
    return false;
}


////////////////////////////////////////////////////////////////////////
//
extern "C"   mem_interface* get_mem_interface()
{
    return &g_mem_interface;
}