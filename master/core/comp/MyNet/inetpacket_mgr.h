#pragma once

/*

   管理封包类 

*/

class net_packet;
class net_conn;

class inetpacket_mgr
{
public:
  inetpacket_mgr(void){}
  virtual ~inetpacket_mgr(void) {}

public:
  virtual bool add_packet(net_packet* pPacket)  = 0;					//添加到封包管理器中去，pPacket是已经处理好的封包了
  virtual net_packet* get_packet(bool bRemoveFlag = true) = 0;		//从封包管理器中获取一个完整的数据包
  virtual bool free_conn_packets(net_conn* pConn) = 0;			//释放一个链接的所有封包

  virtual net_packet* alloc_net_packet() = 0;							//申请一个封包结构
  virtual bool free_net_packet(net_packet* pPacket) = 0;				//释放一个封包结构

protected:

};
