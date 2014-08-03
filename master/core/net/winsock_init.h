// UdpHandler.h: interface for the CUdpHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_Klib_WinSockInit_H)
#define _Klib_WinSockInit_H

#include <WinSock2.h>
#include <windows.h>


/*
* @brief	初始化winsock的类，只需要包含该头文件即可
*/

namespace klib {
namespace net {
    

class winsock_init 
{
public:
	winsock_init() 
    {
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2,2), &wsaData);
	}

    ~winsock_init()
    {
        WSACleanup();
    }
};


}}


#endif 
