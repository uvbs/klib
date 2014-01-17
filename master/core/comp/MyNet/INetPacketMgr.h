#pragma once

/*

   管理封包类 

*/

class NetPacket;
class net_conn;

class INetPacketMgr
{
public:
  INetPacketMgr(void);
  ~INetPacketMgr(void);

public:
  virtual bool AddPacket(NetPacket* pPacket)  = 0;					//添加到封包管理器中去，pPacket是已经处理好的封包了
  virtual NetPacket* GetPacket(bool bRemoveFlag = true) = 0;		//从封包管理器中获取一个完整的数据包
  virtual bool FreeConnPacket(net_conn* pConn) = 0;			//释放一个链接的所有封包

  virtual NetPacket* AllocNetPacket() = 0;							//申请一个封包结构
  virtual bool FreeNetPacket(NetPacket* pPacket) = 0;				//释放一个封包结构

protected:

};
