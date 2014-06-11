#ifndef _klib_udp_socket_h
#define _klib_udp_socket_h

#include <WinSock2.h>
#include <windows.h>


#include "../third/sigslot.h"
#include "../kthread/thread.h"
#include "ip_v4.h"


namespace klib {
namespace net {


///< UDP套接字，用于接收与发送UDP数据包
class udp_socket
{
public:
	udp_socket(void);
	virtual ~udp_socket(void);

public:
    ///< 使用前必须先进行初始化
	BOOL init(USHORT uBindPort = 0);

    ///< 获取绑定的端口
    USHORT  get_bind_port();

    ///< 地址重用
    BOOL enable_reuse(BOOL bReuse = TRUE);

    ///< 启用广播
    BOOL enable_broadcast(BOOL bBroadCast = TRUE);

    ///< 
    BOOL udp_socket::enable_udpreset(BOOL bEnable = FALSE);

    ///< 以异步的方式运行,会创建线程，收到消息后会以信息号的方式发送
    BOOL start_async();

    /*
    * @brief 通知需要停止,可能会花一点时间来等待
    * dwWaitSec  -  需要待待的秒数 
    */
    BOOL stop_async(DWORD dwMilliseconds = INFINITE) ;
    
    ///< 反初始化
	BOOL uninit();

    ///< 获取套接字
	SOCKET& get_socket();

    ///< 设置TTL值
    BOOL set_ttl(UINT uValue);

    ///< 返回TTL值
    UINT get_ttl();

    ///< 设置要发送的目的ip和端口
	BOOL connect(const char* strSvrIp, USHORT uSvrPort);

    ///< 下面的方法要调用connect后才能使用
	int  send(const char* buff, int iLen);

    ///< 通过ip字符串发送数据,端口为主机序
	BOOL send_to(const char* pszSvrIp, USHORT uSvrPort, const char* buff, int iLen); //主机序

    ///< 通过字节序的ip地址发送数据
	BOOL send_to(ip_v4 svrIp, USHORT uSvrPort, const char* buff, int iLen); //网络字节序

    ///< 接收数据
	int recv_from(sockaddr_in* addrFrom, char* buff, int iBuffLen);

    ///< 接收数据信号
    sigslot::signal5<udp_socket*, ip_v4, USHORT, char*, int> sign_msg_recv;

protected:
    ///< 处理函数
    void work_func(void*);

private:
	BOOL		m_bInitSocket;		///< 初始套接字
	BOOL		m_bConnected;		///< udp模式的

	SOCKET		m_socket;			///< 套接字
	USHORT		m_uBindPort;		///< 绑定端口

    klib::kthread::Thread thread_;
};


}}


#endif
