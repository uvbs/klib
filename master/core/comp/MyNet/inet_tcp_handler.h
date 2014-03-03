#pragma once

#include "net_conn.h"

class network_i;


//----------------------------------------------------------------------
// tcp回调处理器
class inet_tcp_handler
{
public:
  virtual bool on_connect(net_conn_ptr pConn, bool bConnected = true) = 0;
  virtual bool on_disconnect(net_conn_ptr pConn) = 0;
  virtual bool on_read(net_conn_ptr pConn, const char* buff, size_t len) = 0;
  virtual bool on_write(net_conn_ptr pConn, size_t len) = 0;
  virtual bool on_accept(net_conn_ptr pListen, net_conn_ptr pNewConn, bool bSuccess = true) = 0;
};