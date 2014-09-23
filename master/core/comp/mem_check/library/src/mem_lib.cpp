#include "mem_lib.h"

#include "../../dll/include/addr_mgr.h"
#include "../../dll/include/allocator.h"

auto_cs g_memoib_cs;

mem_lib* mem_lib::m_instance = nullptr;

mem_lib::mem_lib(void)
    : m_addr_mgr(nullptr)
    , m_enable_stats(true)
{
    if (!m_dll_loader.load_lib(library_dll_ame))
    {
        //@todo 
        return;
    }
    
    get_mem_interface_func func = (get_mem_interface_func) 
        m_dll_loader.get_addr("get_mem_interface");
    if (nullptr == func)
    {
        //@todo 
        return;
    }

    m_mem_i = func();
    m_addr_mgr = m_mem_i->create_mgr();
}

mem_lib::~mem_lib(void)
{
}

mem_lib* mem_lib::instance()
{
    if (nullptr == m_instance)
    {
        auto_lock locker(g_memoib_cs);

        if (nullptr == m_instance)
        {
            mem_lib* p = MemAlloc<mem_lib>().allocate(sizeof(mem_lib));
            new (p) mem_lib;
            m_instance = p;
        }
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

    if (!m_enable_stats)
    {
        return ptr;
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
    if (!m_enable_stats)
    {
        return;
    }

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
        //call global free function
        m_mem_i->global_free_addr(ptr);
    }
    
    return;
}

void mem_lib::enable_stats(bool benable)
{
    m_enable_stats = benable;
}

void mem_lib::set_desc(const char* desc)
{
    m_addr_mgr->set_desc(desc);
}

const char* mem_lib::to_str(char* desc_buff, 
                            size_t buf_size,
                            const char* file_name, 
                            size_t line, 
                            size_t size_type)
{
    if (nullptr == file_name)
    {
        return nullptr;
    }

    char* cur_pos = (char*) strrchr(file_name, '\\');
    if (nullptr == cur_pos)
    {
        cur_pos = strncpy(desc_buff, file_name, buf_size);
    }
    else 
    {
        ++ cur_pos;
        cur_pos = strncpy(desc_buff, cur_pos, buf_size);
    }

    cur_pos = strcat(cur_pos, ":");
        
    char tmp[20];
    _itoa(line, tmp, 10);
    cur_pos = strcat(cur_pos, tmp);

    return desc_buff;
}