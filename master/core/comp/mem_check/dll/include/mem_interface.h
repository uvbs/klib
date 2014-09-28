#ifndef _klib_mem_interface_h_
#define _klib_mem_interface_h_

#include <string>

class addr_mgr;

class mem_interface
{
public:
    // interface define
    virtual addr_mgr*   create_mgr();
    virtual bool        free_addr(addr_mgr* cur, void* ptr);
    virtual bool        stats(char* desc, std::string& str);
};

extern "C" __declspec(dllexport)  mem_interface* get_mem_interface();
typedef mem_interface* (*get_mem_interface_func) ();


#endif // _klib_mem_interface_h_