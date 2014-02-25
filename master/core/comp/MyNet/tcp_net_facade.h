#pragma once

#include "inet_tcp_handler.h"
#include "net_packet.h"


class icombiner;
class inetpacket_mgr;
class dispatcher_handler;
class inet_conn_mgr;


//----------------------------------------------------------------------
// 网络处理客户端接口
//----------------------------------------------------------------------
class tcp_net_facade : public inet_tcp_handler
{
public:
  // 接口设置(必须在调用init前设置)
  virtual bool set_icombiner(icombiner* pCombiner) = 0;
  virtual bool set_dispatch_handler(dispatcher_handler* pHandler) = 0;
  virtual bool init() = 0;

  // 管理接口
  virtual network_i* get_network() = 0;
  virtual inet_conn_mgr* get_net_conn_mgr() = 0;

  // 事件处理器
  virtual bool add_event_handler(inet_tcp_handler* handler) = 0;
  virtual bool del_event_handler(inet_tcp_handler* handler) = 0;

protected:
};