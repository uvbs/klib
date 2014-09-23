#include "../include/mem_mgr.h"


addr_mgr* mem_mgr::create()
{
    addr_mgr* mgr = new addr_mgr;
    if (nullptr == mgr)
    {
        return nullptr;
    }

    m_mgr_arr.push_back(mgr);
    return mgr;
}


simp_string mem_mgr::stats()
{
    simp_string str;

    return std::move(str);
}