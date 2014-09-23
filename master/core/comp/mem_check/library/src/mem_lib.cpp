#include "mem_lib.h"

// #include "../../dll/include/addr_mgr.h"

auto_cs g_memoib_cs;

mem_lib* mem_lib::m_instance = nullptr;

mem_lib::mem_lib(void)
    : m_addr_mgr(nullptr)
{
    //@todo load library and get the function
    HMODULE hmod = LoadLibrary(library_dll_ame);
    if (0 == hmod)
    {
        //@todo 
        return;
    }

    get_mem_interface_func func = (get_mem_interface_func) GetProcAddress(hmod, "get_mem_interface");
    if (nullptr == func)
    {
        //@todo 
        return;
    }

    mem_interface* mem_i = func();
    m_addr_mgr = mem_i->create_mgr();
}

mem_lib::~mem_lib(void)
{
}

mem_lib* mem_lib::instance()
{
    if (nullptr == m_instance)
    {
        g_memoib_cs.lock();

        if (nullptr == m_instance)
        {
            mem_lib* p = MemAlloc<mem_lib>().allocate(sizeof(mem_lib));
            new (p) mem_lib;
            m_instance = p;
        }
        g_memoib_cs.unlock();
    }

    return m_instance;
}

void* mem_lib::alloc_global(size_t type_size, const char* desc /*= nullptr*/)
{
    void* ptr = MemAlloc<void*>().allocate(type_size);
    if (nullptr == ptr)
    {
        return nullptr;
    }

    mem_lib* mlib = mem_lib::instance();
    addr_mgr* mgr = mlib->m_addr_mgr;
    
    bool ret = mgr->add_addr_info(ptr, type_size, desc);
    if (!ret)
    {
        //@todo call global add function

    }
    
    return ptr;
}

void mem_lib::free_global(void* ptr)
{
    if (nullptr == ptr)
    {
        return ;
    }

    mem_lib* mlib = mem_lib::instance();
    addr_mgr* mgr = mlib->m_addr_mgr;
    
    size_t type_size = 0;
    bool ret = mgr->del_addr_info(ptr, type_size);
    if (!ret)
    {
        //@todo call global free function

    }
    
    return;
}

