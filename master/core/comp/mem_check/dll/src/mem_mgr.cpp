#include "../include/mem_mgr.h"
#include "../include/lock.h"

#include <sstream>

mem_mgr* mem_mgr::m_instance = nullptr;
auto_cs     g_mem_mgr_cs;


mem_mgr* mem_mgr::instance()
{
    if (nullptr == m_instance)
    {
        auto_lock  locker(g_mem_mgr_cs);

        if (nullptr == m_instance)
        {
            mem_mgr* mgr = MemAlloc<mem_mgr>().allocate(sizeof(mem_mgr));
            new (mgr) mem_mgr;
            m_instance = mgr;
        }
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

    auto_lock  locker(m_addr_cs);
    m_mgr_arr.push_back(mgr);
    return mgr;
}

bool mem_mgr::free(addr_mgr* cur, void* ptr)
{
    size_t nsize = 0;

    auto_lock locker(m_addr_cs);

    for (auto itr = m_mgr_arr.begin(); 
         itr != m_mgr_arr.end();
         ++ itr)
    {
        if (*itr == cur)
        {
            continue;
        }

        if ((*itr)->del_addr_info(ptr, nsize))
        {
            return true;
        }
    }

    return false;
} 

simp_string mem_mgr::detail(const char* desc)
{
    simp_string str;

    view_addr_callback func = std::bind(&mem_mgr::print_info, 
        this, 
        std::ref(str), 
        std::placeholders::_1);

    for (auto itr = this->m_mgr_arr.begin();
         itr != this->m_mgr_arr.end();
         ++ itr)
    {
        if (nullptr == desc)
        {
            str.append((*itr)->get_desc());
            str.append("\r\n");

            (*itr)->for_each(func);
        }
        else if (strcmp((*itr)->get_desc(), desc) == 0)
        {
            str.append((*itr)->get_desc());
            str.append("\r\n");

            (*itr)->for_each(func);
            break;
        }
    }
    
    return std::move(str);
}

simp_string mem_mgr::stats(const char* desc)
{
    simp_string str;
    addr_mgr* mgr = nullptr;
    std::stringstream ss;

    auto func_format_str = [&](addr_stats_info& info)
    {
        str.append(mgr->get_desc());
        str.append(",");

        ss << info.ncurr_size;
        ss << ",";
        ss << info.nmax_size;
        ss << "\r\n";
        str.append(ss.str().c_str());
        ss.str("");
    };
    
    for (auto itr = this->m_mgr_arr.begin();
         itr != this->m_mgr_arr.end();
         ++ itr)
    {
        mgr = *itr;
        addr_stats_info& info = mgr->get_stats_info();

        if (nullptr == desc)
        {
            func_format_str(info);
        }
        else if (strcmp((*itr)->get_desc(), desc) == 0)
        {
            func_format_str(info);
            break;
        }
    }
    
    return std::move(str);
}

// format test.cs£¨100£¬78£©
void mem_mgr::print_info(simp_string& str, addr_info* info)
{
    char buff[20];

    //str.append("location: ");
    str.append(info->desc);

    str.append(",size:");
    str.append(_itoa(info->nsize, buff, 10));
    str.append("\r\n");
}