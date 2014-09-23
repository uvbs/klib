#ifndef _klib_mem_interface_h_
#define _klib_mem_interface_h_


#include "addr_mgr.h"

class mem_interface
{
public:
    // interface define
    addr_mgr*   create_mgr();
    bool        global_free_addr(void* ptr);
};

extern "C" __declspec(dllexport)  mem_interface* get_mem_interface();
typedef mem_interface* (*get_mem_interface_func) ();


#endif // _klib_mem_interface_h_