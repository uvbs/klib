
#pragma once

class INetConnection;

typedef void (*NetConnMgrCallBackFun)(INetConnection* pConn, void* param);

class INetConnectionMgr
{
public:
  virtual bool AddConnection(INetConnection* pConn) = 0;
  virtual bool RemoveConnection(INetConnection* pConn) = 0;
  virtual bool IsExitsConnection(INetConnection* pConn) = 0;

  virtual int GetConnectionCount() = 0;
  virtual bool ForeachConnection(NetConnMgrCallBackFun* callback, void* param) = 0;
};