#pragma once

#include "INetEventHandler.h"
#include "NetPacket.h"

//----------------------------------------------------------------------
// Summary:
//      网络处理客户端接口
//----------------------------------------------------------------------

class ICombiner;
class INetPacketMgr;
class IDispatchHandler;
class INetConnectionMgr;
class INetTcpClient : public INetEventHandler
{
public:
  virtual bool SetICombiner(ICombiner* pCombiner) = 0;
  virtual bool SetIDispatchHandler(IDispatchHandler* pHandler) = 0;
  virtual bool SetINetConnectionMgr(INetConnectionMgr* pMgr) = 0;
  virtual bool InitClient()  = 0;

  virtual INetNetwork*   GetNetwork() = 0;
  virtual INetPacketMgr* GetNetPacketMgr() = 0;
  virtual INetConnectionMgr* GetINetConnectionMgr() = 0;

  virtual bool AddEventHandler(INetEventHandler* handler) = 0;
  virtual bool RemoveEventHandler(INetEventHandler* handler) = 0;

protected:
};