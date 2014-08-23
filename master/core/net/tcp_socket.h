
#if !defined(_Klib_TcpSoket_H)
#define _Klib_TcpSoket_H


#include <WinSock2.h>
#include <windows.h>


namespace klib {
namespace net {



//TCP套接字类
//使用:
//	tcp_socket  socket;
//  socket.init();
//  socket.connect();
class tcp_socket  
{
public:
    tcp_socket();
    tcp_socket(SOCKET sock, BOOL bConnected);
    virtual ~tcp_socket();

public:
    operator SOCKET& () {  return sock_; }

public:
    BOOL init();              ///< 初始化套接字,如果uPort不为0则绑定
    SOCKET& get_socket();

    BOOL bind_port(USHORT uPort);                    ///< 
    USHORT get_bind_port();                           ///< 返回的端口为主机序

    BOOL listen(int ibacklog = 5);                  ///< 表示监听
    tcp_socket accept();                            ///< 接受连接

    BOOL connect(const char* ip, USHORT uPort);		///< 主机序
    BOOL connect(UINT32  uIP, USHORT uPort);		///< 网络字节序

    BOOL close();                                   ///< 关闭连接
    BOOL is_connected();                              ///< 判断是否连接
    BOOL enable_noblock(BOOL bEnable = TRUE);        ///< 启用非阻塞
    BOOL send(const char* buff, int iLen);          ///< 发送数据
    int  recv(char* buff, int iLenOfBuff, int seconds, int useconds);  ///< seconds = 1 等待1秒

protected:
    SOCKET sock_;                                  ///< 套接字
    BOOL   connected_;                            ///< 表示是否连接上了

};


}}


#endif // !defined(_Klib_TcpSoket_H)
