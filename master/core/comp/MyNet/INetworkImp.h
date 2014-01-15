#pragma once

#include "INetwork.h"
#include "MySocket.h"

#include <list>
#include <Mswsock.h>

/*
  处理网络相关数据
*/

class Klib_OverLapped: public OVERLAPPED
{
public:
  // 以下是用户需要的信息
  bool bFixed; //表示是否是固定的内存，不允许释放

  //外部使用
  int  operate_type;    // 提交的操作类型
  void* pvoid;          // 附带其它数据的
  DWORD dwBytesTransfered;  //传输了的数据
  WSABUF wsaBuf;      //
  char  buff[2 * 1024];			// 传输数据的buf
  //DWORD dwRead; //读取的字节数 //todo 

public:
  Klib_OverLapped() {
    memset(this, 0, sizeof(Klib_OverLapped));
  }
};

class INetNetworkImp : public INetNetwork
{
public:
  INetNetworkImp(void);
  ~INetNetworkImp(void);

public:
  virtual bool InitNetwork() ;   
  virtual bool SetNetEventHandler(INetEventHandler* handler) ;    //设置事件处理接口
  virtual bool RunNetwork() ;   //启动网络层-》创建线程

  virtual INetConnection* PostAccept(INetConnection* pListenConn) ;	//投递接受请求,可多次投递，监听套接字必须用 CreateListenConn 创建
  virtual bool PostConnection(INetConnection* pConn) ;		//投递连接请求
  virtual bool PostRead(INetConnection* pConn) ;	//投递读请求，在接受连接后底层会默认投递一个请求
  virtual bool PostWrite(INetConnection* pConn, const char* buff, size_t len) ;	//投递写请求

  virtual INetConnection* CreateListenConn(USHORT uLocalPort) ;	//创建返回监听套接字，返回一个INetConnection结构，用于接受连接
  virtual INetConnection* CreateNewConn() ;			//创建一个连接套接字
  virtual bool ReleaseConnection(INetConnection* pConn);		//释放连接

protected:
  static unsigned int WINAPI ThreadNetwork(void* param);

  void InitFixedOverlappedList(size_t nCount);    //初始固定的Overlapped的个数，这部分内存不能被释放
  Klib_OverLapped* GetMyOverlapped();                //申请重叠结构
  bool ReleaseMyOverlapped(Klib_OverLapped* pMyoverlapped);	//释放重叠结构

  void CheckAndDisconnect(INetConnection* pConn);     //判断在套接字上还有没有未处理的投递请求，如果没有了则断开连接

private:
  HANDLE m_hIocp;   ///< 完成端口句柄
  INetEventHandler* m_INetEventHandler;   ///< 移交上层处理的接口
  LPFN_ACCEPTEX m_lpfnAcceptEx;   ///< AcceptEx函数指针
  
  typedef std::list<Klib_OverLapped*> OverLappedListType;  /// 保存Klib_OverLapped的链表类型
  OverLappedListType m_overlappedList;      /// 申明链表，用来保存Klib_OverLapped
  auto_cs            m_csOverlappedList;      /// 同步访问m_overlappedList

  typedef std::list<INetConnection*> INetConnListType;  /// 接口列表类型定义
  INetConnListType m_INetConnFreeList;      /// 保存网络连接接口链表
  auto_cs          m_CsNetFreeList;         /// 同步访问m_INetConnFreeList
};