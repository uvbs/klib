#pragma once

class INetConnection;
class INetNetwork;
class INetEventHandler
{
public:
  virtual bool OnConnect(INetConnection* pConn, bool bConnected = true) = 0;
  virtual bool OnDisConnect(INetConnection* pConn) = 0;
  virtual bool OnRead(INetConnection* pConn, const char* buff, size_t len) = 0;
  virtual bool OnWrite(INetConnection* pConn) = 0;
  virtual bool OnAccept(INetConnection* pListen, INetConnection* pNewConn, bool bSuccess = true) = 0;
};