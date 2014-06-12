#ifndef _klib_ip_seg_table_h
#define _klib_ip_seg_table_h

#include "../Net/ip_v4.h"
#include <set>
using std::set;

namespace klib {
namespace algo {
    
using namespace klib::net;


// Ip段结构体定义
struct ip_seg
{
    // 注:请填主机序的Ip, 网络序的大小比较会出问题    
    ip_v4    ulStartIp;         
    ip_v4    ulEndIp;

    //      用于Set的小于号重载    
    bool operator < (const ip_seg &stCmpElem) const
    {
        return ulEndIp < stCmpElem.ulStartIp;
    }
};


class ip_seg_table
{
public:
    ip_seg_table(void) { clear(); }
    ~ip_seg_table(void) { clear(); }

public:
    // 清空Ip表
    void clear() { m_IpTable.clear(); }

    // 插入一个Ip段, 返回是否成功
    bool insert(const ip_seg &stElem)
    {
        // 不合法的Ip段
        if (stElem.ulStartIp > stElem.ulEndIp)
        {
            return false;
        }
        return m_IpTable.insert(stElem).second;           // 往表中插入一个Ip段，出现Ip段重叠时会失败
    }
        
    // 同上
    bool insert(ip_v4 ulStartIp, ip_v4 ulEndIp)
    {
        ip_seg   stTempElem;
        stTempElem.ulStartIp = ulStartIp;
        stTempElem.ulEndIp   = ulEndIp;
        return insert(stTempElem);
    }
        
    // 插入一个ip地址段
    bool insert(const char* pszStartIp, const char* pszEndIp) 
    {
        ip_seg  stTempElem;
        stTempElem.ulStartIp = ntohl(inet_addr(pszStartIp));
        stTempElem.ulEndIp   = ntohl(inet_addr(pszEndIp));
        return insert(stTempElem);
    }

    
    //    
    bool insert(const char* pszStartIp, UINT uIpCount)
    {
        ip_seg  stTempElem;
        stTempElem.ulStartIp  = ntohl(inet_addr(pszStartIp));
        stTempElem.ulEndIp    = stTempElem.ulStartIp + uIpCount - 1;
        return insert(stTempElem);
    }
        
    // 删除包含该Ip的Ip段, 返回是否成功
    bool remove(const ip_v4 ulIp)
    {
        ip_seg   stTempElem;
        stTempElem.ulStartIp = stTempElem.ulEndIp = ulIp;

        return 1 == m_IpTable.erase(stTempElem);
    }

    //  查找包含该Ip的Ip段
    //  找到返回该Ip段的指针 (请勿直接修改指针指向的数据)
    //  否则返回NULL
    const ip_seg * find(const ip_v4 ulIp)
    {
        ip_seg   stTempElem;
        stTempElem.ulStartIp = stTempElem.ulEndIp = ulIp;
        set <ip_seg>::iterator itr = m_IpTable.find(stTempElem);

        return itr == m_IpTable.end() ? NULL : &*itr;
    }

protected:
    // 存储Ip的Set表
    std::set <ip_seg>         m_IpTable;            
};


}}

#endif
