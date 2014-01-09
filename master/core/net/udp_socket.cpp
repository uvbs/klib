//#include "StdAfx.h"

#include "udp_socket.h"
#include "addr_resolver.h"

#include <process.h>
#include <Ws2tcpip.h>   //IP_TTL

using namespace klib::net;

udp_socket::udp_socket(void)
{
	m_socket = INVALID_SOCKET;
	m_uBindPort = 0;	
	m_bConnected = FALSE;
	m_bInitSocket = FALSE;

}

udp_socket::~udp_socket(void)
{
    if (m_socket != INVALID_SOCKET) 
    {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
}

BOOL udp_socket::init(USHORT uBindPort)
{
	if (m_bInitSocket && m_socket != INVALID_SOCKET) 
    {
		return TRUE;
	}

	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_socket == INVALID_SOCKET) 
    {
		return FALSE;
	}

	if (uBindPort > 0)
	{
		sockaddr_in addrLocal;
		memset(&addrLocal, 0, sizeof(addrLocal));
		addrLocal.sin_addr.s_addr	= 0;
		addrLocal.sin_family		= AF_INET;
		addrLocal.sin_port			= htons(uBindPort);
		int ret = bind(m_socket, (struct sockaddr*)&addrLocal, sizeof(addrLocal));
		if (SOCKET_ERROR == ret) 
        {
			return FALSE;
		}
        m_uBindPort = uBindPort;
	}

	m_bInitSocket = TRUE;
	return TRUE;
}

USHORT  udp_socket::get_bind_port()
{
    return m_uBindPort;
}

BOOL udp_socket::enable_reuse(BOOL bReuse/* = TRUE*/)
{
    BOOL bOpt = bReuse;
    if (SOCKET_ERROR == setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&bOpt, sizeof(bOpt)))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL udp_socket::enable_broadcast(BOOL bBroadCast/* = TRUE*/)
{
    BOOL bOpt = bBroadCast;
    if (SOCKET_ERROR == setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt)))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL udp_socket::start_async()
{
    klib::kthread::Thread::thread_func_type f = std::bind(&udp_socket::work_func, this, std::tr1::placeholders::_1);
    thread_.start(f, NULL);

    return TRUE;
}

BOOL udp_socket::stop_async(DWORD dwMilliseconds/* = INFINITE*/) 
{
    thread_.exit(true);
    thread_.wait();

    return TRUE;
}

BOOL udp_socket::uninit()
{
	closesocket(m_socket);
	m_bInitSocket = FALSE;
	m_bConnected = FALSE;
	m_socket = INVALID_SOCKET;

	return TRUE;
}

SOCKET& udp_socket::get_socket()
{
	return m_socket;
}

BOOL udp_socket::set_ttl(UINT uValue)
{
    int ret = ::setsockopt(m_socket, IPPROTO_IP, IP_TTL, (char*)&uValue, sizeof(uValue));
    return ret != SOCKET_ERROR;
}

UINT udp_socket::get_ttl()
{
    UINT uTTL = 0;
    int iLen = sizeof(uTTL);
    int ret = ::getsockopt(m_socket, IPPROTO_IP, IP_TTL, (char*)&uTTL, &iLen);
    if (SOCKET_ERROR != ret) 
    {
        return uTTL;
    }
    return 0;
}

BOOL udp_socket::connect(const char* strSvrIp, USHORT uSvrPort)
{
	addr_resolver resolver(strSvrIp);

	if (resolver.size() <= 0)
    {
		return FALSE;
	}

	sockaddr_in addrSvr = {0};
	addrSvr.sin_addr.s_addr = resolver.at(0);
	addrSvr.sin_port = htons(uSvrPort);
	addrSvr.sin_family = AF_INET;

	int ret = ::connect(m_socket, (sockaddr*)&addrSvr, sizeof(addrSvr));
	if (SOCKET_ERROR == ret) 
    {
		return FALSE;
	}

	m_bConnected = TRUE;
	return TRUE;
}

int udp_socket::send(const char* buff, int iLen)
{
	if (!m_bConnected) 
    {
		return SOCKET_ERROR;
	}

	int ret = ::send(m_socket, buff, iLen, 0);
	if (SOCKET_ERROR == ret) 
    {
		return SOCKET_ERROR;
	}

	return ret;
}

BOOL udp_socket::send_to(const char* strSvrIp, USHORT uSvrPort, const char* buff, int iLen)  //主机序
{
	addr_resolver resolver(strSvrIp);

	if (resolver.size() <= 0) 
    {
		return FALSE;
	}

	return send_to(
		(DWORD)resolver.at(0),
		htons(uSvrPort),
		buff,
		iLen);
}

BOOL udp_socket::send_to(ip_v4 svrIp, USHORT uSvrPort, const char* buff, int iLen) //网络字节序
{
	sockaddr_in svrAddr = {0};
	svrAddr.sin_addr.s_addr = svrIp;
	svrAddr.sin_port = uSvrPort;
	svrAddr.sin_family = AF_INET;

	int ret = ::sendto(m_socket, buff, iLen, 0, (sockaddr*)&svrAddr, sizeof(svrAddr));
	if (SOCKET_ERROR == ret) 
    {
		return FALSE;
	}

	return TRUE;
}

int udp_socket::recv_from(sockaddr_in* addrFrom, char* buff, int iBuffLen)
{
	int addrLen = sizeof(sockaddr_in);
	int ret = SOCKET_ERROR;

	if (addrFrom)
	{
		ret = recvfrom(m_socket, buff, iBuffLen, 0, (sockaddr*)addrFrom, &addrLen);
	}
	else 
    {
		sockaddr_in addr = {0};
		ret = recvfrom(m_socket, buff, iBuffLen, 0, (sockaddr*)&addr, &addrLen);
	}

	return ret;
}

void udp_socket::work_func(void*)
{
    DWORD dwWaitRet = 0;
    int   iRecvRet = 0;
    char  buffer[2048];
    sockaddr_in addrfrom;
    int len	= sizeof(sockaddr_in);

    while (TRUE)
    {
        if (thread_.state() == klib::kthread::Thread::TS_EXIT) 
        {
            break;
        }

        iRecvRet = recv_from(&addrfrom, buffer, sizeof(buffer));
        if(iRecvRet == SOCKET_ERROR || iRecvRet == 0 ) 
        {
            continue;
        }
        buffer[iRecvRet] = '\0';

        //发送信号
        SignMsgRecv(addrfrom.sin_addr.s_addr,
                    addrfrom.sin_port,
                    buffer,
                    iRecvRet);
    }
}