// UdpClient.cpp: implementation of the udp_client class.
//
//////////////////////////////////////////////////////////////////////

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
	socket_					= INVALID_SOCKET;
	stop_					= TRUE;
	sock_inited_			= FALSE;
}

udp_client::~udp_client()
{
	if (socket_ != INVALID_SOCKET)
	{
		closesocket(socket_);
		socket_ = INVALID_SOCKET;
	}
}

BOOL udp_client::init(USHORT port/* = 0*/ ) 
{
	if (sock_inited_ && socket_ != INVALID_SOCKET) 
    {
		return TRUE;
	}
	
	socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socket_ == INVALID_SOCKET) {
		return FALSE;
	}

	if (port > 0)
	{
		if (!bind_port(port))
        {
            return FALSE;
        }
	}
	
	sock_inited_ = TRUE;
	return TRUE;
}

BOOL udp_client::reinit()
{
    if (sock_inited_) 
    {
        _ASSERT(socket_ != INVALID_SOCKET);
        closesocket(socket_);
        socket_ = INVALID_SOCKET;

        sock_inited_ = FALSE;
    }

    return init();
}

BOOL udp_client::bind_port(USHORT port) 
{
    _ASSERT(port > 0 );
    sockaddr_in addrLocal;
    memset(&addrLocal, 0, sizeof(addrLocal));
    addrLocal.sin_addr.s_addr	= 0;
    addrLocal.sin_family		= AF_INET;
    addrLocal.sin_port			= htons(port);
    int ret = bind(socket_, (struct sockaddr*)&addrLocal, sizeof(addrLocal));
    if (SOCKET_ERROR == ret) {
        return FALSE;
    }

    return TRUE;
}

BOOL udp_client::enable_reuse(BOOL bReuse/* = TRUE*/) 
{
    BOOL bOpt = bReuse;
    if (SOCKET_ERROR == setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (char*)&bOpt, sizeof(bOpt)))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL udp_client::enable_broadcast(BOOL bBroadCast/* = TRUE*/) 
{
    BOOL bOpt = bBroadCast;
    if (SOCKET_ERROR == setsockopt(socket_, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt)))
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
    status = WSAIoctl(socket_, SIO_UDP_CONNRESET,
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
	stop_ = bStop;
}

BOOL udp_client::is_stop() 
{
	return stop_;
}

void udp_client::set_callback(udp_client_callback callback)
{
	udp_callback_ = callback;
}

BOOL udp_client::send_to(ip_v4 uAddr, USHORT port, const char*buff, int iLen)
{
	sockaddr_in addrTo;
	memset(&addrTo, 0, sizeof(addrTo));
	addrTo.sin_addr.s_addr = uAddr;
	addrTo.sin_family = AF_INET;
	addrTo.sin_port = port;

	return SOCKET_ERROR != sendto(socket_, buff, iLen, 0, (sockaddr*)&addrTo, sizeof(addrTo));
}

void udp_client::set_svr_info(ip_v4 uAddrServer, USHORT uPortServer) 
{
	svr_addr_ = uAddrServer;
	svr_port_ = uPortServer;
}

BOOL udp_client::send_to_svr(const char* buff, int iLen)
{
	if (!(svr_addr_ && svr_port_)) 
    {
		return FALSE;
	}

	return this->send_to(svr_addr_, svr_port_, buff, iLen);
}

BOOL udp_client::send_to(const char* str_addr, USHORT port, const char*buff, int iLen)
{
	addr_resolver resolver(str_addr);

	if (resolver.size() <= 0)
	{
		return FALSE;
	}

	return this->send_to(resolver.at(0), htons(port), buff, iLen);
}

void udp_client::work_thread(void* param)
{
    this->do_server();
}

BOOL udp_client::do_server()
{
	if (!init()) {
		return FALSE;
	}
	
	char buff[2*1024] = {0};
	sockaddr_in  addrFrom;
	int lenOfAddr = sizeof(addrFrom);
	int ret = 0;
	while (TRUE) 
    {
		ret = recvfrom(socket_, buff, sizeof(buff), 0, (struct sockaddr*)&addrFrom, &lenOfAddr);
		if (ret > 0)
		{
			if (udp_callback_)
			{
				udp_callback_(this,
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
