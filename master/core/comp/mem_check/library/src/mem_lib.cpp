#include "mem_lib.h"


auto_cs g_memoib_cs;

mem_lib* mem_lib::m_instance = nullptr;

mem_lib::mem_lib(void)
    : m_addr_mgr(nullptr)
{

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
            mem_lib* p = new mem_lib;
            m_instance = p;
        }
        g_memoib_cs.unlock();
    }

    return m_instance;
}