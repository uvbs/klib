#include "addr_mgr.h"


//------------------------------------------------------------------------

bool addr_mgr::add_addr_info(void* p, size_t nsize)
{
    addr_info info;
    info.nsize = nsize;

    auto itr = m_addr_infos.find(p);
    if (itr == m_addr_infos.end()) 
    {
        m_addr_infos.insert(std::make_pair(p, info));
        return true;
    }
    return false;
}

bool addr_mgr::del_addr_info(void* p, size_t& nsize)
{
    auto itr = m_addr_infos.find(p);
    if (itr != m_addr_infos.end()) 
    {
        nsize = itr->second.nsize;
        m_addr_infos.erase(itr);
        return true;
    }

    return false;
}