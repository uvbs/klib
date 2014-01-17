#pragma once

#include "inet_conn_mgr.h"
#include <list>
#include <map>


#define  NETCONNECTION_ARRAY_LENGTH 9

class inet_conn_mgr_imp : public inet_conn_mgr
{
public:
  inet_conn_mgr_imp(void);
  ~inet_conn_mgr_imp(void);

public:
  virtual bool add_conn(net_conn* pConn) ;
  virtual bool del_conn(net_conn* pConn) ;
  virtual bool is_exist_conn(net_conn* pConn) ;

  virtual int get_conn_count() ;
  virtual bool for_each_conn(conn_callback* callback, void* param) ;

protected:
  int ConnHashFun(void* param, int len);

protected:
  typedef std::map<net_conn*, net_conn*> ConnectionListType;
  ConnectionListType m_ConnList[NETCONNECTION_ARRAY_LENGTH];
  mutex      mutexs_[NETCONNECTION_ARRAY_LENGTH];

};
