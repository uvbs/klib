#pragma once
#pragma warning(disable: 4996)

#include "MySocket.h"
#include <string>
#include <list>
#include <kthread/auto_lock.h>

using namespace klib::kthread;


#define  MAX_CLIENT_BUFF_LEN 5*1024

enum emOperationType
{
  OP_NONE,
  OP_ACCEPT,
  OP_READ,
  OP_WRITE,
  OP_CONNECT,
};


class NetPacket;
class INetConnection
{
public:
  INetConnection(void);
  ~INetConnection(void);
  
public:
  inline NetSocket& GetSocket() { return m_socket; }
  bool InitPeerInfo();  // 初始对端信息，通过getpeername来获取
  inline void SetPeerAddress(const char* straddr) { strncpy(m_strAddress, straddr, sizeof(m_strAddress)-1); }
  char* GetPeerAddress() 
  {
	  if (m_strAddress[0] == '\0') {
		  SetPeerAddress(inet_ntoa(*(in_addr*) &m_PeerAddr));
	  }
	  return m_strAddress;
  }

  inline void SetPeerPort(USHORT port) { m_PeerPort = port; }
  inline USHORT GetPeerPort() { return m_PeerPort; }
  inline void SetLocalPort(USHORT port) { m_LocalPort = port; }
  inline USHORT GetLocalPort() { return m_LocalPort; }

  inline void DisConnect() {  closesocket(m_socket); m_socket = INVALID_SOCKET; }

  bool AddStream(const char* buff, int len) ;
  bool GetStream(char* buff, int len);
  inline char* GetBuff() { return mbuff; }
  inline int  GetDataLen() {  return datalen; }

  inline DWORD GetLastActiveTimestamp() { return m_tLastActive; }
  void UpdateLastActiveTimestamp() ;

  void AddPostReadCount();
  void DecPostReadCount();
  USHORT GetPostReadCount();
  void AddPostWriteCount();
  void DecPostWriteCount();
  USHORT GetPostWriteCount();

  inline BOOL GetIsClosing() { return m_bClosing; }
  inline void SetIsClosing(BOOL bClose = TRUE) { m_bClosing = bClose; }

  inline void SetKey(DWORD dwKey) { m_dwKey = dwKey; }
  inline DWORD GetKey() { return m_dwKey; }

  inline DWORD GetBytesWrite() { return m_dwBytesWrite; }
  inline DWORD AddBytesWrite(DWORD dwBytes) { return (m_dwBytesWrite+=dwBytes); }
  inline DWORD GetBytesRead() { return m_dwBytesRead; }
  inline DWORD AddBytesRead(DWORD dwBytes) { return m_dwBytesRead += dwBytes; }

  void lock() { mutex_.lock(); }
  void unlock(){ mutex_.unlock() ;}

protected:
  NetSocket m_socket;		//套接字
  DWORD     m_tLastActive;		//最后活跃时间

  USHORT m_PeerPort;		//对端端口,主机序
  DWORD  m_PeerAddr;		//对端地址，网络字节序

  USHORT m_LocalPort;     //本地端口，做监听用的
  USHORT  m_PostReadCount;       //投递接收的数量
  USHORT  m_PostWriteCount;       //投递发送的数量
  BOOL   m_bClosing;            //指示是否是在关闭
  DWORD  m_dwKey;           //绑定的键值
  DWORD  m_dwBytesWrite;     //输出了多少字节数
  DWORD  m_dwBytesRead;     //接收到了多少字节数
  mutex  mutex_;        //临界区对象，用于互斥数据的访问
  char   m_strAddress[50];		//字符串地址
  int datalen;					//缓冲区中保存数据的长度
  char  mbuff[MAX_CLIENT_BUFF_LEN];		//该连接的数据缓冲区
};