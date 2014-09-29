#include "../include/addr_mgr.h"


//------------------------------------------------------------------------
addr_mgr::addr_mgr()
    : m_enable_stats(true)
{
    this->m_desc[0] = 0;
}

void addr_mgr::set_desc(const char* desc)
{
    strncpy(this->m_desc, desc, _countof(this->m_desc));
}

const char* addr_mgr::get_desc()
{
    return this->m_desc;
}

void addr_mgr::enable_stats(bool enable)
{
    m_enable_stats = enable;
}

bool addr_mgr::get_enable_stats()
{
    return m_enable_stats;
}

bool addr_mgr::add_addr_info(void* p, 
                             size_t nsize,
                             const char* desc)
{
    if (!m_enable_stats)
        return false;

    auto_lock locker(m_auto_cs);
    auto itr = m_addr_infos.find(p);
    if (itr == m_addr_infos.end()) 
    {
        addr_info& info = m_addr_infos[p];
        info.nsize = nsize;
        if (desc)
            strncpy(info.desc, desc, _countof(info.desc));

        // update stats info
        m_stats_info.nalloc_count ++ ;
        m_stats_info.ncurr_size += nsize;

        if (m_stats_info.ncurr_size > m_stats_info.nmax_size) 
        {
            m_stats_info.nmax_size = m_stats_info.ncurr_size;
        }

        m_stats_info.self_size += sizeof(addr_info) + sizeof(std::pair<void*, addr_info>);
        return true;
    }

    return false;
}

bool addr_mgr::del_addr_info(void* p, 
                             size_t& nsize,
                             const char* desc)
{
    if (!m_enable_stats)
        return false;

    auto_lock locker(m_auto_cs);

    auto itr = m_addr_infos.find(p);
    if (itr != m_addr_infos.end()) 
    {
        nsize = itr->second.nsize;
        m_addr_infos.erase(itr);

        m_stats_info.nfree_count ++;
        m_stats_info.ncurr_size -= nsize;

        m_stats_info.self_size -= sizeof(addr_info) + sizeof(std::pair<void*, addr_info>);

        return true;
    }

    return false;
}

void addr_mgr::for_each(const view_addr_callback& func)
{
    auto_lock locker(m_auto_cs);

    auto itr = m_addr_infos.begin();
    for ( ; itr != m_addr_infos.end(); ++ itr)
    {
        func (&itr->second);
    }
}