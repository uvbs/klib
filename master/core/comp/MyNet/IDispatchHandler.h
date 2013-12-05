#pragma once

class NetPacket;

//处理封包的
class IDispatchHandler
{
public:
  virtual bool DispatchPacket(NetPacket* pPacket) = 0;
  //virtual 
};