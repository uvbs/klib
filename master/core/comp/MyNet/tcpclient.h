#pragma once

#include "inet_event_handler.h"
#include "net_packet.h"

//----------------------------------------------------------------------
// Summary:
//      网络处理客户端接口
//----------------------------------------------------------------------

class icombiner;
class inetpacket_mgr;
class dispatcher_handler;
class inet_conn_mgr;
class tcpclient : public inet_event_handler
{
public:
  virtual bool SetICombiner(icombiner* pCombiner) = 0;
  virtual bool SetIDispatchHandler(dispatcher_handler* pHandler) = 0;
  virtual bool SetINetConnectionMgr(inet_conn_mgr* pMgr) = 0;
  virtual bool InitClient()  = 0;

  virtual inetwork*   GetNetwork() = 0;
  virtual inetpacket_mgr* GetNetPacketMgr() = 0;
  virtual inet_conn_mgr* GetINetConnectionMgr() = 0;

  virtual bool AddEventHandler(inet_event_handler* handler) = 0;
  virtual bool RemoveEventHandler(inet_event_handler* handler) = 0;

protected:
};