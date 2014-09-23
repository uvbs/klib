#ifndef _klib_addr_helper_h_
#define _klib_addr_helper_h_

#include <WinSock2.h>
#include <Windows.h>

#include "ip_v4.h"



namespace klib {
namespace net {


// 需要先初始化网络库 WSAStartup
class addr_helper
{
public:
    static bool get_ipv4_lst(ipv4_list_type& lst);
    static bool get_conn_ip(SOCKET s, ip_v4& ip);
    static bool get_peer_ip(SOCKET s, ip_v4& ip);
    
};




}}



#endif // _klib_addr_helper_h_