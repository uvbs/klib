#pragma once

class net_conn;
class inetwork;
class inet_tcp_handler
{
public:
  virtual bool on_connect(net_conn* pConn, bool bConnected = true) = 0;
  virtual bool on_disconnect(net_conn* pConn) = 0;
  virtual bool on_read(net_conn* pConn, const char* buff, size_t len) = 0;
  virtual bool on_write(net_conn* pConn, size_t len) = 0;
  virtual bool on_accept(net_conn* pListen, net_conn* pNewConn, bool bSuccess = true) = 0;
};