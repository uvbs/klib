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
  virtual bool add_conn(INetConnection* pConn) ;
  virtual bool rmv_conn(INetConnection* pConn) ;
  virtual bool is_exist_conn(INetConnection* pConn) ;

  virtual int get_conn_count() ;
  virtual bool for_each_conn(conn_callback* callback, void* param) ;

protected:
  int ConnHashFun(void* param, int len);

protected:
  typedef std::map<INetConnection*, INetConnection*> ConnectionListType;
  ConnectionListType m_ConnList[NETCONNECTION_ARRAY_LENGTH];
  mutex      mutexs_[NETCONNECTION_ARRAY_LENGTH];

};
