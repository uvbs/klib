#include "addr_helper.h"

#include "../macro.h"

using namespace klib;
using namespace klib::net;


bool addr_helper::get_ipv4_lst(ipv4_list_type& lst)
{
    char host_name[256];
    if(gethostname(host_name, sizeof(host_name)) == SOCKET_ERROR)  
    {
        return false;
    }
    KLIB_ASSERT(WSANOTINITIALISED != WSAGetLastError());

    struct hostent *host = gethostbyname(host_name);
    if(nullptr == host)
    {
        return false;
    }

    int index  = 0;
    while (host->h_addr_list[index])
    {
        ip_v4 ip;
        memcpy(ip.get_ip_buf(), 
            host->h_addr_list[index],
            sizeof(struct in_addr));
        lst.push_back(ip);

        ++ index;
    }

    return (!lst.empty());
}

bool addr_helper::get_conn_ip(SOCKET s, ip_v4& ip)
{
    sockaddr_in local_addr;
    int addr_len = sizeof(local_addr);

    int ret = getsockname(s, (sockaddr*)&local_addr, &addr_len);
    if (SOCKET_ERROR == ret) {
        KLIB_ASSERT(WSANOTINITIALISED != WSAGetLastError());
        return false;
    }

    ip.set_val(local_addr.sin_addr.s_addr);

    return true;
}

bool addr_helper::get_peer_ip(SOCKET s, ip_v4& ip)
{
    sockaddr_in local_addr;
    int addr_len = sizeof(local_addr);

    int ret = getpeername(s, (sockaddr*)&local_addr, &addr_len);
    if (SOCKET_ERROR == ret) {
        KLIB_ASSERT(WSANOTINITIALISED != WSAGetLastError());
        return false;
    }

    ip.set_val(local_addr.sin_addr.s_addr);

    return true;
}