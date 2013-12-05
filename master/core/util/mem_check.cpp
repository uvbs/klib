//#include "stdafx.h"
#include "mem_check.h"

namespace klib{
namespace mem
{

bool g_enable_mem_stats = true;

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

//------------------------------------------------------------------------

class_mem_mgr::~class_mem_mgr()
{

}

class_mem_mgr* class_mem_mgr::instance()
{
    static class_mem_mgr _instance;
    return & _instance;
}

auto_cs& class_mem_mgr::get_lock()
{
    return m_cs;
}

void class_mem_mgr::add_class_mem(const char* class_name, void* p, size_t nsize)
{
    if (!g_enable_mem_stats) 
    {
        return;
    }
    
    addr_mgr_.add_addr_info(p, nsize);

    auto itr = m_class_infos.find(class_name);
    if (itr == m_class_infos.end()) 
    {
        class_mem_info info;
        info.ncurr_size = nsize;
        info.nitem_size = nsize;
        info.nmax_size  = nsize;
        info.nitem_count = 1;
        info.nalloc_count ++;

        strncpy(info.class_name, class_name, _countof(info.class_name));

        m_class_infos.insert(std::make_pair(class_name, info));
    }
    else
    {
        class_mem_info& info = itr->second;

        info.ncurr_size += nsize;
        info.nitem_count++;
        info.nalloc_count ++;

        if (info.ncurr_size > info.nmax_size) 
        {
            info.nmax_size = info.ncurr_size;
        }
    }
}

void class_mem_mgr::del_class_mem(const char* class_name, void* p)
{
    if (!g_enable_mem_stats) 
    {
        return;
    }

    size_t nsize = 0;
    
    addr_mgr_.del_addr_info(p, nsize);

    auto itr = m_class_infos.find(class_name);
    if (itr != m_class_infos.end()) 
    {
        class_mem_info& info = itr->second;

        info.ncurr_size -= nsize;
        info.nitem_count --;
        info.nfree_count ++;
    }
}

std::string class_mem_mgr::summary()
{
    typedef std::basic_string<char, std::char_traits<char>, MemAlloc<char> > simp_string;

    simp_string strinfo = "class memory stats: \r\n";
    char szbuff[100] = {0};
    {
        auto_lock lock_1(m_cs);

        for (auto itr=m_class_infos.begin(); itr != m_class_infos.end(); ++ itr) 
        {
            strinfo += itr->first;
            class_mem_info& info = itr->second;

            strinfo.append("\r\nncurr_size: ");
            strinfo.append(_i64toa( info.ncurr_size, szbuff, 10));
            strinfo.append("\r\n");

            strinfo.append("nmax_size: ");
            strinfo.append(_i64toa( info.nmax_size, szbuff, 10));
            strinfo.append("\r\n");

            strinfo.append("item_size: ");
            strinfo.append(_i64toa( info.nitem_size, szbuff, 10));
            strinfo.append("\r\n");

            strinfo.append("item_count: ");
            strinfo.append(_i64toa( info.nitem_count, szbuff, 10));
            strinfo.append("\r\n");

            strinfo.append("nalloc_count: ");
            strinfo.append(_i64toa( info.nalloc_count, szbuff, 10));
            strinfo.append("\r\n");

            strinfo.append("nfree_count: ");
            strinfo.append(_i64toa( info.nfree_count, szbuff, 10));
            strinfo.append("\r\n");
            strinfo.append("\r\n");
        }
    }

    return std::string(strinfo.c_str());
}

//------------------------------------------------------------------------

global_mem_mgr* global_mem_mgr::instance()
{
    static global_mem_mgr _instance;
    return & _instance;
}

auto_cs& global_mem_mgr::get_lock()
{
    return m_cs;
}

void global_mem_mgr::add_global_mem(void* p, size_t nsize)
{
    if (!g_enable_mem_stats) 
    {
        return;
    }

    if (addr_mgr_.add_addr_info(p, nsize))
    {
        mem_info_.nalloc_count ++ ;
        mem_info_.ncurr_size += nsize;

        if (mem_info_.ncurr_size > mem_info_.nmax_size) 
        {
            mem_info_.nmax_size = mem_info_.ncurr_size;
        }
    }
}

void global_mem_mgr::del_global_mem(void* p)
{
    if (!g_enable_mem_stats) 
    {
        return;
    }

    size_t nsize = 0;

    addr_mgr_.del_addr_info(p, nsize);

    mem_info_.nfree_count ++;
    mem_info_.ncurr_size -= nsize;
}

std::string global_mem_mgr::summary() const
{
    auto_lock lock(m_cs);

    typedef std::basic_string<char, std::char_traits<char>, MemAlloc<char> > simp_string;
    simp_string strinfo;
    char szbuff[100] = {0};

    {
        strinfo += "global memory stats: \r\n";

        strinfo.append("curr_size: ");
        strinfo.append(_i64toa( mem_info_.ncurr_size, szbuff, 10));
        strinfo.append("\r\n");

        strinfo.append("max_size: ");
        strinfo.append(_i64toa( mem_info_.nmax_size, szbuff, 10));
        strinfo.append("\r\n");

        strinfo.append("alloc_count: ");
        strinfo.append(_i64toa( mem_info_.nalloc_count, szbuff, 10));
        strinfo.append("\r\n");

        strinfo.append("free_count: ");
        strinfo.append(_i64toa( mem_info_.nfree_count, szbuff, 10));
        strinfo.append("\r\n");
    }

    return std::move(strinfo.c_str());
}

std::string global_mem_mgr::summary_xml(const char* module_name) const
{
    auto info = mem_info_;

    std::string strInfo = module_name;
    strInfo += ",";
    strInfo += std::to_string(info.ncurr_size);
    strInfo += ",";
    strInfo += std::to_string(info.nmax_size);

    return std::move(strInfo);
}

//------------------------------------------------------------------------
// memory alloc
void* mem_imp::alloc_class(const char* class_name, size_t nsize) // 类名与大小
{
    if (!g_enable_mem_stats) 
    {
        return malloc(nsize);
    }

    class_mem_mgr* mgr_ = class_mem_mgr::instance();
    auto_lock lock(mgr_->get_lock());

    void* p = malloc(nsize);
    if (nullptr == p) 
        return nullptr;
    
    mgr_->add_class_mem(class_name, p, nsize);
    return p;
}

void* mem_imp::alloc_global(size_t nsize) // 全局的
{
    if (!g_enable_mem_stats) 
    {
        return malloc(nsize);
    }

    global_mem_mgr* mgr_ = global_mem_mgr::instance();
    auto_lock lock(mgr_->get_lock());

    void* p = malloc(nsize);
    if (nullptr == p) 
        return nullptr;


    mgr_->add_global_mem(p, nsize);

    return p;
}

//------------------------------------------------------------------------
// memory free
void mem_imp::free_class(const char* class_name, void* p)
{
    if (!g_enable_mem_stats) 
    {
        free(p);
        return;
    }

    class_mem_mgr* mgr_ = class_mem_mgr::instance();
    auto_lock lock(mgr_->get_lock());

    free(p);

    mgr_->del_class_mem(class_name, p);
}

void mem_imp::free_global(void* p)
{
    if (!g_enable_mem_stats) 
    {
        free(p);
        return;
    }

    global_mem_mgr* mgr_ = global_mem_mgr::instance();
    auto_lock lock(mgr_->get_lock());

    free(p);

    mgr_->del_global_mem(p);
}

// switch
void mem_imp::enable_stats(bool benable)
{
    g_enable_mem_stats = benable;
}

// summary
std::string mem_imp::summary()
{
    mem::class_mem_mgr* mgr_ = mem::class_mem_mgr::instance();
    mem::global_mem_mgr* global_mgr_ = mem::global_mem_mgr::instance();

    std::string strInfo = "\r\n";

    strInfo += mgr_->summary();
    strInfo += global_mgr_->summary();

    return std::move(strInfo);
}





}}
