
#pragma once

class net_conn;

typedef void (*conn_callback)(net_conn* pConn, void* param);

class INetConnectionMgr
{
public:
  virtual bool add_conn(net_conn* pConn) = 0;
  virtual bool rmv_conn(net_conn* pConn) = 0;
  virtual bool is_exist_conn(net_conn* pConn) = 0;

  virtual int get_conn_count() = 0;
  virtual bool for_each_conn(conn_callback* callback, void* param) = 0;
};