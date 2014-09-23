#ifndef _klib_addr_mgr_h_
#define _klib_addr_mgr_h_

#include <map>

#include "../include/allocator.h"
#include "../include/lock.h"

#define  ADDR_DESC_LEN  (50)

// stats memory info
struct addr_stats_info
{
    addr_stats_info() 
        : ncurr_size(0)
        , nmax_size(0)
        , nalloc_count(0)
        , nfree_count(0) 
    {}

    size_t ncurr_size;           // 当前大小
    size_t nmax_size;            // 最大大小

    size_t nalloc_count;          // 申请的次数
    size_t nfree_count;          // 释放的次数
};

struct addr_info
{
    addr_info() 
        : nsize(0)
    { desc[0] = 0; }

    size_t nsize;
    char   desc[ADDR_DESC_LEN];
};


// 统计
class addr_mgr
{
public:
    addr_mgr();

    void set_desc(const char* desc);

    bool add_addr_info(void* p, 
        size_t nsize,
        const char* desc = nullptr);

    bool del_addr_info(void* p, 
        size_t& nsize,
        const char* desc = nullptr);

protected:
    std::map<void*, 
             addr_info, 
             std::less<void*>, 
             MemAlloc<std::pair<void*, addr_info>> >    m_addr_infos;

    auto_cs          m_auto_cs;
    addr_stats_info  m_stats_info;
    char             m_desc[ADDR_DESC_LEN];
};



#endif // _klib_addr_mgr_h_