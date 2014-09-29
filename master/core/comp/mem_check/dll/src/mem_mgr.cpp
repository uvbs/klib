#include "../include/mem_mgr.h"
#include "../include/lock.h"

#include <sstream>

mem_mgr* mem_mgr::m_instance = nullptr;
auto_cs     g_mem_mgr_cs;

mem_mgr::mem_mgr()
    : m_enable_stats(true)
{
}

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
    mgr->enable_stats(m_enable_stats);

    auto_lock  locker(m_addr_cs);
    m_mgr_arr.push_back(mgr);
    return mgr;
}

bool mem_mgr::free(addr_mgr* cur, void* ptr)
{
    size_t nsize = 0;
    addr_mgr* mgr = nullptr;

    auto_lock locker(m_addr_cs);

    for (auto itr = m_mgr_arr.begin(); 
         itr != m_mgr_arr.end();
         ++ itr)
    {
        mgr = *itr;
        if (mgr == cur)
        {
            continue;
        }

        if (mgr->del_addr_info(ptr, nsize))
        {
            return true;
        }
    }

    return false;
} 

void mem_mgr::enable_stats(bool enable)
{
    m_enable_stats = enable;
    
    addr_mgr* mgr = nullptr;

    auto_lock locker(m_addr_cs);

    for (auto itr = m_mgr_arr.begin(); 
         itr != m_mgr_arr.end();
         ++ itr)
    {
        mgr = *itr;
        
        mgr->enable_stats(enable);
    }
}

simp_string mem_mgr::debug_output(const char* desc)
{
    simp_string str;
    char buff[20];

    view_addr_callback func = [&](addr_info* info)
    {
        str.append(info->desc);

        str.append(",size:");
        str.append(_itoa(info->nsize, buff, 10));
        str.append("\r\n");

        OutputDebugStringA(str.c_str());
        str.clear();
    };
    
    for (auto itr = this->m_mgr_arr.begin();
         itr != this->m_mgr_arr.end();
         ++ itr)
    {
        if (nullptr == desc)
        {
            str.append("module:");
            str.append((*itr)->get_desc());
            str.append("\r\n");

            (*itr)->for_each(func);
        }
        else if (strcmp((*itr)->get_desc(), desc) == 0)
        {
            str.append("module:");
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

bool mem_mgr::write_file(char* desc, const char* filename)
{
    addr_mgr* mgr = nullptr;
    char  buff[25];

    FILE* hfile = fopen(filename, "wb");
    if (nullptr == hfile)
    {
        return false;
    }
    std::shared_ptr<FILE> f_releasor(hfile, [&](FILE* hfile){ fclose(hfile); });

    auto func_write_addr_info = [&](addr_mgr* mgr)
    {
        fwrite(mgr->get_desc(), 1, strlen(mgr->get_desc()), hfile);
        fwrite("\r\n", 1, 2, hfile);
        mgr->for_each([&](addr_info* info)
        {
            fwrite(info->desc, 1, strlen(info->desc), hfile);
            _itoa(info->nsize, buff, 10);
            fwrite(buff, 1, strlen(buff), hfile);
            fwrite("\r\n", 1, 2, hfile);
        });
    };
    
    for (auto itr = this->m_mgr_arr.begin();
         itr != this->m_mgr_arr.end();
         ++ itr)
    {
        mgr = *itr;

        if (nullptr == desc)
        {
            func_write_addr_info(mgr);
        }
        else if (strcmp((*itr)->get_desc(), desc) == 0)
        {
            func_write_addr_info(mgr);
            break;
        }
    }
    return true;
}

bool mem_mgr::for_each(const view_addr_info_func& func)
{
    addr_mgr* mgr = nullptr;

    for (auto itr = this->m_mgr_arr.begin();
         itr != this->m_mgr_arr.end();
         ++ itr)
    {
        mgr = *itr;
        func(mgr->get_desc(), mgr->get_stats_info());
    }
    return true;
}
