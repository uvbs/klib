#pragma once

#include "INetConnectionMgr.h"
#include <list>
#include <map>

#define  NETCONNECTION_ARRAY_LENGTH 9

class INetConnectionMgrImp : public INetConnectionMgr
{
public:
  INetConnectionMgrImp(void);
  ~INetConnectionMgrImp(void);

public:
  virtual bool AddConnection(INetConnection* pConn) ;
  virtual bool RemoveConnection(INetConnection* pConn) ;
  virtual bool IsExitsConnection(INetConnection* pConn) ;

  virtual int GetConnectionCount() ;
  virtual bool ForeachConnection(NetConnMgrCallBackFun* callback, void* param) ;

protected:
  int ConnHashFun(void* param, int len);

protected:
  typedef std::map<INetConnection*, INetConnection*> ConnectionListType;
  ConnectionListType m_ConnList[NETCONNECTION_ARRAY_LENGTH];
  auto_cs      m_cs[NETCONNECTION_ARRAY_LENGTH];

};
