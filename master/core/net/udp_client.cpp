// UdpClient.cpp: implementation of the udp_client class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "udp_client.h"

#include "../kthread/auto_lock.h"
#include "addr_resolver.h"

#include <PROCESS.H>
#include <ASSERT.H>

using namespace klib::net;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

udp_client::udp_client()
{
	m_sock					= INVALID_SOCKET;
	udp_handler_			= NULL;
	m_bStop					= TRUE;
	m_bSocketInit			= FALSE;
}

udp_client::~udp_client()
{
	if (m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
}

BOOL udp_client::init(USHORT uPort/* = 0*/ ) 
{
	if (m_bSocketInit && m_sock != INVALID_SOCKET) 
    {
		return TRUE;
	}
	
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_sock == INVALID_SOCKET) {
		return FALSE;
	}

	if (uPort > 0)
	{
		if (!bind_port(uPort))
        {
            return FALSE;
        }
	}
	
	m_bSocketInit = TRUE;
	return TRUE;
}

BOOL udp_client::reinit()
{
    if (m_bSocketInit) 
    {
        _ASSERT(m_sock != INVALID_SOCKET);
        closesocket(m_sock);
        m_sock = INVALID_SOCKET;

        m_bSocketInit = FALSE;
    }

    return init();
}

BOOL udp_client::bind_port(USHORT uPort) 
{
    _ASSERT(uPort > 0 );
    sockaddr_in addrLocal;
    memset(&addrLocal, 0, sizeof(addrLocal));
    addrLocal.sin_addr.s_addr	= 0;
    addrLocal.sin_family		= AF_INET;
    addrLocal.sin_port			= htons(uPort);
    int ret = bind(m_sock, (struct sockaddr*)&addrLocal, sizeof(addrLocal));
    if (SOCKET_ERROR == ret) {
        return FALSE;
    }

    return TRUE;
}

BOOL udp_client::enable_reuse(BOOL bReuse/* = TRUE*/) 
{
    BOOL bOpt = bReuse;
    if (SOCKET_ERROR == setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&bOpt, sizeof(bOpt)))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL udp_client::enable_broadcast(BOOL bBroadCast/* = TRUE*/) 
{
    BOOL bOpt = bBroadCast;
    if (SOCKET_ERROR == setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt)))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL udp_client::enable_udpreset(BOOL bEnable/* = FALSE*/)
{
    // 以下解决10054问题， ref: http://blog.csdn.net/ccnucjp8136/article/details/4515002
    DWORD dwBytesReturned = 0;
    BOOL bNewBehavior = FALSE;
    DWORD status;
#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR,12)
    // disable  new behavior using
    // IOCTL: SIO_UDP_CONNRESET
    status = WSAIoctl(m_sock, SIO_UDP_CONNRESET,
        &bNewBehavior, sizeof(bNewBehavior),
        NULL, 0, &dwBytesReturned,
        NULL, NULL);
    if (SOCKET_ERROR == status)
    {
        DWORD dwErr = WSAGetLastError();
        if (WSAEWOULDBLOCK == dwErr)
        {
            // nothing to do
            return(FALSE);
        }
        else
        {
            printf("WSAIoctl(SIO_UDP_CONNRESET) Error: %d/n", dwErr);
            return(FALSE);
        }
    }

    return TRUE;
}

BOOL udp_client::start(BOOL bBlock)
{
	this->stop(FALSE);
    
	if (bBlock)	
    {
		this->do_server();
	}
	else 
    {
        klib::kthread::Thread::thread_func_type fun = std::bind(&udp_client::work_thread, this, std::tr1::placeholders::_1);
        thread_.start(fun, NULL);
	}

	return TRUE;
}

void udp_client::stop(BOOL bStop/* = TRUE*/)
{
	m_bStop = bStop;
}

BOOL udp_client::is_stop() 
{
	return m_bStop;
}

void udp_client::set_handler(udp_handler* handler_)
{
	udp_handler_ = handler_;
}

BOOL udp_client::send_to(UINT32 uAddr, USHORT uPort, const char*buff, int iLen)
{
	sockaddr_in addrTo;
	memset(&addrTo, 0, sizeof(addrTo));
	addrTo.sin_addr.s_addr = uAddr;
	addrTo.sin_family = AF_INET;
	addrTo.sin_port = uPort;

	return SOCKET_ERROR != sendto(m_sock, buff, iLen, 0, (sockaddr*)&addrTo, sizeof(addrTo));
}

void udp_client::set_svr_info(UINT32 uAddrServer, USHORT uPortServer) 
{
	m_uSvrAddr = uAddrServer;
	m_uSvrPort = uPortServer;
}

BOOL udp_client::send_to_svr(const char* buff, int iLen)
{
	if (!(m_uSvrAddr && m_uSvrPort)) 
    {
		return FALSE;
	}

	return this->send_to(m_uSvrAddr, m_uSvrPort, buff, iLen);
}

BOOL udp_client::send_to(const char* strAddr, USHORT uPort, const char*buff, int iLen)
{
	addr_resolver resolver(strAddr);

	if (resolver.size() <= 0)
	{
		return FALSE;
	}

	return this->send_to(resolver.at(0), htons(uPort), buff, iLen);
}

void udp_client::work_thread(void* param)
{
    this->do_server();
}

BOOL udp_client::do_server()
{
	if (!init()) 
    {
		return FALSE;
	}
	
	char buff[2*1024] = {0};
	sockaddr_in  addrFrom;
	int lenOfAddr = sizeof(addrFrom);
	int ret = 0;
	while (TRUE) 
    {
		ret = recvfrom(m_sock, buff, sizeof(buff), 0, (struct sockaddr*)&addrFrom, &lenOfAddr);
		if (ret > 0)
		{
			if (udp_handler_)
			{
				udp_handler_->on_msg(this,
                                     addrFrom.sin_addr.s_addr, 
									 addrFrom.sin_port,	
									 buff, 
									 ret);
			}
		}
		else {
			Sleep(100);
		}
	}


	return TRUE;
}
