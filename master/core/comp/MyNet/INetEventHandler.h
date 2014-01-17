#pragma once

class net_conn;
class INetNetwork;
class INetEventHandler
{
public:
  virtual bool OnConnect(net_conn* pConn, bool bConnected = true) = 0;
  virtual bool OnDisConnect(net_conn* pConn) = 0;
  virtual bool OnRead(net_conn* pConn, const char* buff, size_t len) = 0;
  virtual bool OnWrite(net_conn* pConn) = 0;
  virtual bool OnAccept(net_conn* pListen, net_conn* pNewConn, bool bSuccess = true) = 0;
};