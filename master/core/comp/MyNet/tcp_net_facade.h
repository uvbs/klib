#pragma once

#include "inet_event_handler.h"
#include "net_packet.h"


class icombiner;
class inetpacket_mgr;
class dispatcher_handler;
class inet_conn_mgr;


//----------------------------------------------------------------------
// 网络处理客户端接口
//----------------------------------------------------------------------
class tcp_net_facade : public inet_event_handler
{
public:
  virtual bool set_icombiner(icombiner* pCombiner) = 0;
  virtual bool set_dispatch_handler(dispatcher_handler* pHandler) = 0;
  virtual bool set_net_conn_mgr(inet_conn_mgr* pMgr) = 0;
  virtual bool init_client()  = 0;

  virtual inetwork*   get_network() = 0;
  virtual inetpacket_mgr* get_net_packet_mgr() = 0;
  virtual inet_conn_mgr* get_net_conn_mgr() = 0;

  virtual bool add_event_handler(inet_event_handler* handler) = 0;
  virtual bool del_event_handler(inet_event_handler* handler) = 0;

protected:
};