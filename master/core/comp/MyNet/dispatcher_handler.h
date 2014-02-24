#pragma once

#include "net_packet.h"

//处理封包的
class dispatcher_handler
{
public:
  virtual bool dispatch_packet(net_packet* pPacket) = 0;
  //virtual 
};