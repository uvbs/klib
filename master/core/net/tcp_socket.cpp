// TCPSocket.cpp: implementation of the tcp_socket class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "tcp_socket.h"

#include "addr_resolver.h"

using namespace klib::net;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

tcp_socket::tcp_socket()
{
	m_sock			= INVALID_SOCKET;
	m_bConnected	= FALSE;
}

tcp_socket::tcp_socket(SOCKET sock, BOOL bConnected)
{
    m_sock = sock;
    m_bConnected = bConnected;
}

tcp_socket::~tcp_socket()
{
    close();
}

BOOL tcp_socket::init()
{
	if((m_sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == INVALID_SOCKET)
    {    
        return FALSE;   
    }

	return TRUE;
}

SOCKET& tcp_socket::get_socket()
{
    return m_sock;
}

BOOL tcp_socket::bind_port(USHORT uPort)
{
    sockaddr_in sockaddr;
    memset(&sockaddr, 0, sizeof(sockaddr_in));    

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);    
    sockaddr.sin_port = htons(uPort);

    if(bind(m_sock, (struct sockaddr*) &sockaddr, sizeof(sockaddr)) == SOCKET_ERROR) 
    {
        return FALSE;
    }
    return TRUE;
}

USHORT tcp_socket::get_bind_port()
{
    sockaddr_in addr;
    int addrlen = sizeof(addr);
    int ret = getsockname(m_sock, (sockaddr*) &addr, &addrlen);
    if (SOCKET_ERROR == ret) 
    {
        return 0;
    }

    return ntohs(addr.sin_port);
}

BOOL tcp_socket::listen(int ibacklog)
{
	if (SOCKET_ERROR == ::listen(m_sock, ibacklog)) 
    {
		return FALSE;
	}
	return TRUE;
}

tcp_socket tcp_socket::accept()
{
    sockaddr_in local = {0};
    local.sin_family = AF_INET;
    int len = sizeof(sockaddr);
    SOCKET connectsocket = ::accept(m_sock, (sockaddr*) &local, &len);

    DWORD dwErr = WSAGetLastError();
    if (INVALID_SOCKET == connectsocket) 
    {
        return tcp_socket(connectsocket, FALSE);
    }
    return tcp_socket(connectsocket, TRUE);
}

BOOL tcp_socket::connect(const char* ip, USHORT uPort)
{
	if (NULL == ip || 0 == uPort) 
    {
		return FALSE;
	}

	DWORD dwIP = 0;
	addr_resolver resolver(ip);
	if (resolver.size() > 0) 
    {
		dwIP = resolver.at(0);
	}
	else 
    {
		return FALSE;
	}

	return connect(dwIP, htons(uPort));
}

BOOL tcp_socket::connect(UINT32  uIP, USHORT uPort)
{
	sockaddr_in addrSvr;
	memset(&addrSvr, 0, sizeof(addrSvr));
	addrSvr.sin_family			 = AF_INET;
	addrSvr.sin_port			 = uPort;
	addrSvr.sin_addr.s_addr		 = uIP;
	
	int ret = ::connect(m_sock, (struct sockaddr*) &addrSvr, sizeof(sockaddr));
	if (SOCKET_ERROR == ret) 
    {
		return FALSE;
	}
	
	m_bConnected = TRUE;
	return TRUE;
}

BOOL tcp_socket::close()
{
    if (INVALID_SOCKET != m_sock) 
    {
        closesocket(m_sock);
        m_sock				= INVALID_SOCKET;
        m_bConnected		= FALSE;
    }
	return TRUE;
}

BOOL tcp_socket::is_connected()
{
	return m_bConnected;
}

BOOL tcp_socket::enable_noblock(BOOL bEnable /* = TRUE */)
{
	//用非阻塞的连接
	u_long ul= bEnable ? 1 : 0;
    if (SOCKET_ERROR == ioctlsocket(m_sock, FIONBIO, &ul)) 
    {
		return FALSE;
	}
	return TRUE;
}

BOOL tcp_socket::send(const char* buff, int iLen)
{
	enable_noblock(FALSE);

	int ret = ::send(m_sock, buff, iLen, 0);
	if (SOCKET_ERROR == ret) 
    {
		return FALSE;
	}

	return TRUE;
}

int tcp_socket::recv(char* buff, int iLenOfBuff, int seconds, int useconds)
{
	if (seconds == 0 && useconds == 0) 
    {
		this->enable_noblock(FALSE);
		int ret = ::recv(m_sock, buff, iLenOfBuff, 0);
		if (0 == ret || SOCKET_ERROR == ret) 
        {
			m_bConnected = FALSE;
			closesocket(m_sock);
			m_sock = INVALID_SOCKET;
			return ret;
		}
		return ret;
	}
	else 
    {
		this->enable_noblock(TRUE);
		
		fd_set rfd;					// 描述符集 这个将用来测试有没有一个可用的连接
		struct timeval timeout;
		FD_ZERO(&rfd);				//总是这样先清空一个描述符集
		
		timeout.tv_sec	= seconds;    //等下select用到这个
		timeout.tv_usec = useconds;
		
		//现在开始用select
		FD_SET(m_sock, &rfd);    //把sock放入要测试的描述符集 就是说把sock放入了rfd里面 这样下一步调用select对rfd进行测试的时候就会测试sock了(因为我们将sock放入的rdf) 一个描述符集可以包含多个被测试的描述符, 
		int ret = ::select(0, &rfd, 0, 0, &timeout);
		
		if (0 == ret) 
        {
			//time out
			return 0;
		}
		else if (SOCKET_ERROR == ret) 
        {
			closesocket(m_sock);
			m_sock = INVALID_SOCKET;
			m_bConnected = FALSE;
			return SOCKET_ERROR;
		}
		
		if(FD_ISSET(m_sock, &rfd)) 
        {
			//ok
			int ret = ::recv(m_sock, buff, iLenOfBuff, 0);
			if (0 == ret || SOCKET_ERROR == ret) {
				closesocket(m_sock);
				m_sock = INVALID_SOCKET;
				m_bConnected = FALSE;
			}
			return ret;
		}

		return 0;
	}
}