
#pragma once

class INetConnection;

typedef void (*conn_callback)(INetConnection* pConn, void* param);

class INetConnectionMgr
{
public:
  virtual bool add_conn(INetConnection* pConn) = 0;
  virtual bool rmv_conn(INetConnection* pConn) = 0;
  virtual bool is_exist_conn(INetConnection* pConn) = 0;

  virtual int get_conn_count() = 0;
  virtual bool for_each_conn(conn_callback* callback, void* param) = 0;
};