#include "../include/mem_mgr.h"
#include "../include/lock.h"


mem_mgr* mem_mgr::m_instance = nullptr;
auto_cs     g_mem_mgr_cs;


mem_mgr* mem_mgr::instance()
{
    if (nullptr == m_instance)
    {
        g_mem_mgr_cs.lock();

        if (nullptr == m_instance)
        {
            mem_mgr* mgr = MemAlloc<mem_mgr>().allocate(sizeof(mem_mgr));
            new (mgr) mem_mgr;
            m_instance = mgr;
        }

        g_mem_mgr_cs.unlock();
    }

    return m_instance;
}

addr_mgr* mem_mgr::create()
{
    addr_mgr* mgr = MemAlloc<addr_mgr>().allocate(sizeof(addr_mgr));
    if (nullptr == mgr)
    {
        return nullptr;
    }

    new (mgr) addr_mgr;
    m_mgr_arr.push_back(mgr);
    return mgr;
}


simp_string mem_mgr::stats()
{
    simp_string str;

    return std::move(str);
}