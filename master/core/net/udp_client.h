// UdpClient.h: interface for the udp_client class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UDPCLIENT_H__E02768E0_20F8_4E86_B807_FDEFD7BF7D67__INCLUDED_)
#define AFX_UDPCLIENT_H__E02768E0_20F8_4E86_B807_FDEFD7BF7D67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _MSC_VER
#include <WinSock2.h>
#include <windows.h>
#endif

#include "../kthread/thread.h"
#include "ip_v4.h"

#include <list>
#include <functional>

namespace klib {
namespace net{

class udp_client;

typedef std::function<void (udp_client* client_, 
    ip_v4  uAddr, 
    USHORT uPort, 
    char* buff, 
    int iLen)> udp_client_callback;

class udp_client
{
public:
	udp_client();
	virtual ~udp_client();

public:
	virtual BOOL init(USHORT uPort = 0) ;     ///< 初始化套接字
    virtual BOOL reinit();                    ///< 重新初始化套接字，主要是为了绑定不同的端口
    virtual BOOL bind_port(USHORT uPort) ;           ///< 帮定特定的端口
    virtual BOOL enable_reuse(BOOL bReuse = TRUE) ;
    virtual BOOL enable_broadcast(BOOL bBroadCast = TRUE) ;
    virtual BOOL enable_udpreset(BOOL bEnable = FALSE);
	
	virtual BOOL start(BOOL bBlock);
	virtual void stop(BOOL bStop = TRUE);
	virtual BOOL is_stop() ;
	virtual void set_callback(udp_client_callback callback);

	virtual void set_svr_info(ip_v4 uAddrServer, USHORT uPortServer) ;//地址和端口为网络序
	virtual BOOL send_to(ip_v4 uAddr, USHORT uPort, const char* buff, int iLen);//地址和端口为网络序
	virtual BOOL send_to(const char* strAddr, USHORT uPort, const char*buff, int iLen);//端口为主机序
	virtual BOOL send_to_svr(const char* buff, int iLen);

private:
	void work_thread(void* param);

	BOOL do_server();

protected:
	//----------------------------------------------------------------------
	SOCKET  m_sock;                         ///< 套接字
	BOOL    m_bStop;                        ///< 是否停止
	BOOL	m_bSocketInit;                  ///< 套接字是否初始化了

	//----------------------------------------------------------------------
	//以下变量都是网络字节序
	ip_v4  m_uSvrAddr;						///< 服务器地址
	USHORT m_uSvrPort;						///< 服务器端口

	udp_client_callback    udp_callback_;	///< UDP事件回调接口
    klib::kthread::Thread  thread_;         ///< 线程
};


}}



#endif // !defined(AFX_UDPCLIENT_H__E02768E0_20F8_4E86_B807_FDEFD7BF7D67__INCLUDED_)
