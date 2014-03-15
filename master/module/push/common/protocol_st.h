#ifndef _PROTOCOL_DEF_H_
#define _PROTOCOL_DEF_H_

#include "stdio.h"
#include "protocol.h"
#include <net/proto/net_archive.h>
#include <net/proto/command_header.h>

using namespace klib::net;

// #pragma pack(1)

/////////////////////////////////////////////////////////////////////////
//开始定义协议: C2S 
//// 查询逻辑服务器的地址
struct PT_CMD_QUERY_LOGIC_SERVER {
	PT_CMD_QUERY_LOGIC_SERVER ()  {
		//this->ver = ADVERTISE_VERSION;
		//this->cmd = CMD_QUERY_LOGIC_SERVER;
		//this->encrypt = E_None;
	}
	
	friend net_archive& operator << (net_archive& ar, PT_CMD_QUERY_LOGIC_SERVER& pt) {
		//ar << *(COMMAND_HEADER*) &pt;
	
		ar << pt.uuid; 
		return ar;
	}
	
	friend net_archive& operator >> (net_archive& ar, PT_CMD_QUERY_LOGIC_SERVER& pt) {
		//ar >> *(COMMAND_HEADER*) &pt;
		
		ar >> pt.uuid; 
		return ar;
	}
	
	
	UINT  uuid;  
};


//// 返回的空闲的逻辑服务器的IP+PORT
struct PT_CMD_QUERY_LOGIC_SERVER_ACK {
	PT_CMD_QUERY_LOGIC_SERVER_ACK ()  {
		//this->ver = ADVERTISE_VERSION;
		//this->cmd = CMD_QUERY_LOGIC_SERVER_ACK;
		//this->encrypt = E_None;
	}
	
	friend net_archive& operator << (net_archive& ar, PT_CMD_QUERY_LOGIC_SERVER_ACK& pt) {
		//ar << *(COMMAND_HEADER*) &pt;
	
		ar << pt.uLogicIP; 
		ar << pt.uLogicUdpPort; 
		ar << pt.uLogicTcpPort; 
		return ar;
	}
	
	friend net_archive& operator >> (net_archive& ar, PT_CMD_QUERY_LOGIC_SERVER_ACK& pt) {
		//ar >> *(COMMAND_HEADER*) &pt;
		
		ar >> pt.uLogicIP; 
		ar >> pt.uLogicUdpPort; 
		ar >> pt.uLogicTcpPort; 
		return ar;
	}
	
	
	UINT  uLogicIP;  
	USHORT  uLogicUdpPort;  
	USHORT  uLogicTcpPort;  
};


//// 查询当前最新版本
struct PT_CMD_QUERY_CURRENT_VERSION {
	PT_CMD_QUERY_CURRENT_VERSION ()  {
		//this->ver = ADVERTISE_VERSION;
		//this->cmd = CMD_QUERY_CURRENT_VERSION;
		//this->encrypt = E_None;
	}
	
	friend net_archive& operator << (net_archive& ar, PT_CMD_QUERY_CURRENT_VERSION& pt) {
		//ar << *(COMMAND_HEADER*) &pt;
	
		ar << pt.uCurVerValue; 
		ar << pt.strVersion; 
		return ar;
	}
	
	friend net_archive& operator >> (net_archive& ar, PT_CMD_QUERY_CURRENT_VERSION& pt) {
		//ar >> *(COMMAND_HEADER*) &pt;
		
		ar >> pt.uCurVerValue; 
		ar >> pt.strVersion; 
		return ar;
	}
	
	
	UINT  uCurVerValue;   	//  当前版本值
	std::string  strVersion;   	//  版本的字符串
};


//// 返回当前的版本号
struct PT_CMD_QUERY_CURRENT_VERSION_ACK {
	PT_CMD_QUERY_CURRENT_VERSION_ACK ()  {
		//this->ver = ADVERTISE_VERSION;
		//this->cmd = CMD_QUERY_CURRENT_VERSION_ACK;
		//this->encrypt = E_None;
	}
	
	friend net_archive& operator << (net_archive& ar, PT_CMD_QUERY_CURRENT_VERSION_ACK& pt) {
		//ar << *(COMMAND_HEADER*) &pt;
	
		ar << pt.strVersion; 
		ar << pt.uVersionValue; 
		ar << pt.strUpdateUrl; 
		ar << pt.strUpdateSign; 
		ar << pt.bForceUpdate; 
		ar << pt.bClientRequest; 
		return ar;
	}
	
	friend net_archive& operator >> (net_archive& ar, PT_CMD_QUERY_CURRENT_VERSION_ACK& pt) {
		//ar >> *(COMMAND_HEADER*) &pt;
		
		ar >> pt.strVersion; 
		ar >> pt.uVersionValue; 
		ar >> pt.strUpdateUrl; 
		ar >> pt.strUpdateSign; 
		ar >> pt.bForceUpdate; 
		ar >> pt.bClientRequest; 
		return ar;
	}
	
	
	std::string  strVersion;   	//  版本的字符串值
	UINT  uVersionValue;   	//  版本的数字值
	std::string  strUpdateUrl;   	//  更新包的地址
	std::string  strUpdateSign;   	//  数字签名
	BOOL  bForceUpdate;   	//  强制更新
	BOOL  bClientRequest;   	//  是否是客户端主动请求的
};


//// 客衣端通知服务器在线消息
struct PT_CMD_ONLINE {
	PT_CMD_ONLINE ()  {
		//this->ver = ADVERTISE_VERSION;
		//this->cmd = CMD_ONLINE;
		//this->encrypt = E_None;
	}
	
	friend net_archive& operator << (net_archive& ar, PT_CMD_ONLINE& pt) {
		//ar << *(COMMAND_HEADER*) &pt;
	
		ar << pt.uid; 
		ar << pt.account; 
		ar << pt.mac; 
		ar << pt.channel; 
		ar << pt.last_msg_id; 
		ar << pt.login_name; 
		ar << pt.version; 
		return ar;
	}
	
	friend net_archive& operator >> (net_archive& ar, PT_CMD_ONLINE& pt) {
		//ar >> *(COMMAND_HEADER*) &pt;
		
		ar >> pt.uid; 
		ar >> pt.account; 
		ar >> pt.mac; 
		ar >> pt.channel; 
		ar >> pt.last_msg_id; 
		ar >> pt.login_name; 
		ar >> pt.version; 
		return ar;
	}
	
	
	UINT64  uid;  
	std::string  account;   	//  用户的帐号(注册的用户)
	std::string  mac;   	//  mac地址
	std::string  channel;   	//  渠道
	UINT64  last_msg_id;   	//  最后收到的消息ID
	std::string  login_name;   	//  电脑的登陆名
	UINT  version;   	//  版本值，用数值比较更好些
};


//// 在线消息的回复消息
struct PT_CMD_ONLINE_ACK {
	PT_CMD_ONLINE_ACK ()  {
		//this->ver = ADVERTISE_VERSION;
		//this->cmd = CMD_ONLINE_ACK;
		//this->encrypt = E_None;
	}
	
	friend net_archive& operator << (net_archive& ar, PT_CMD_ONLINE_ACK& pt) {
		//ar << *(COMMAND_HEADER*) &pt;
	
		ar << pt.bStartMini; 
		ar << pt.uuid; 
		ar << pt.msgId; 
		return ar;
	}
	
	friend net_archive& operator >> (net_archive& ar, PT_CMD_ONLINE_ACK& pt) {
		//ar >> *(COMMAND_HEADER*) &pt;
		
		ar >> pt.bStartMini; 
		ar >> pt.uuid; 
		ar >> pt.msgId; 
		return ar;
	}
	
	
	BOOL  bStartMini;   	//  打开迷你首页
	UINT64  uuid;   	//  生成的唯一标识的ID号
	UINT64  msgId;   	//  需要展示的消息ID
};


//// 离线消息，客户端退出的时候发送
struct PT_CMD_OFFLINE {
	PT_CMD_OFFLINE ()  {
		//this->ver = ADVERTISE_VERSION;
		//this->cmd = CMD_OFFLINE;
		//this->encrypt = E_None;
	}
	
	friend net_archive& operator << (net_archive& ar, PT_CMD_OFFLINE& pt) {
		//ar << *(COMMAND_HEADER*) &pt;
	
		ar << pt.uuid; 
		return ar;
	}
	
	friend net_archive& operator >> (net_archive& ar, PT_CMD_OFFLINE& pt) {
		//ar >> *(COMMAND_HEADER*) &pt;
		
		ar >> pt.uuid; 
		return ar;
	}
	
	
	UINT64  uuid;   	//  生成的唯一标识的ID号
};


//// 离线消息的回复消息，服务器可以选择不回该消息
struct PT_CMD_OFFLINE_ACK {
	PT_CMD_OFFLINE_ACK ()  {
		//this->ver = ADVERTISE_VERSION;
		//this->cmd = CMD_OFFLINE_ACK;
		//this->encrypt = E_None;
	}
	
	friend net_archive& operator << (net_archive& ar, PT_CMD_OFFLINE_ACK& pt) {
		//ar << *(COMMAND_HEADER*) &pt;
	
		
		return ar;
	}
	
	friend net_archive& operator >> (net_archive& ar, PT_CMD_OFFLINE_ACK& pt) {
		//ar >> *(COMMAND_HEADER*) &pt;
		
		
		return ar;
	}
	
	
	
};


//// 消息通知协议
struct PT_CMD_MESSAGE_NOTIFY {
	PT_CMD_MESSAGE_NOTIFY ()  {
		//this->ver = ADVERTISE_VERSION;
		//this->cmd = CMD_MESSAGE_NOTIFY;
		//this->encrypt = E_None;
	}
	
	friend net_archive& operator << (net_archive& ar, PT_CMD_MESSAGE_NOTIFY& pt) {
		//ar << *(COMMAND_HEADER*) &pt;
	
		ar << pt.msgID; 
		return ar;
	}
	
	friend net_archive& operator >> (net_archive& ar, PT_CMD_MESSAGE_NOTIFY& pt) {
		//ar >> *(COMMAND_HEADER*) &pt;
		
		ar >> pt.msgID; 
		return ar;
	}
	
	
	UINT64  msgID;   	//  消息ID
};


//// 消息内容协议
struct PT_CMD_MESSAGE_CONTENT {
	PT_CMD_MESSAGE_CONTENT ()  {
		//this->ver = ADVERTISE_VERSION;
		//this->cmd = CMD_MESSAGE_CONTENT;
		//this->encrypt = E_None;
	}
	
	friend net_archive& operator << (net_archive& ar, PT_CMD_MESSAGE_CONTENT& pt) {
		//ar << *(COMMAND_HEADER*) &pt;
	
		ar << pt.uMsgID; 
		ar << pt.uMsgType; 
		ar << pt.uDelayFetch; 
		ar << pt.uDelayShow; 
		ar << pt.uShowTime; 
		ar << pt.strSign; 
		ar << pt.strMsgContent; 
		return ar;
	}
	
	friend net_archive& operator >> (net_archive& ar, PT_CMD_MESSAGE_CONTENT& pt) {
		//ar >> *(COMMAND_HEADER*) &pt;
		
		ar >> pt.uMsgID; 
		ar >> pt.uMsgType; 
		ar >> pt.uDelayFetch; 
		ar >> pt.uDelayShow; 
		ar >> pt.uShowTime; 
		ar >> pt.strSign; 
		ar >> pt.strMsgContent; 
		return ar;
	}
	
	
	UINT64  uMsgID;   	//  消息ID
	UINT32  uMsgType;   	//  消息类型
	UINT32  uDelayFetch;   	//  延迟多长时间获取消息
	UINT32  uDelayShow;   	//  显示延迟时间（即指需要延迟多少时间显示）
	UINT32  uShowTime;   	//  显示的时长（如果为0则一直显示）
	std::string  strSign;   	//  消息内容的签名（计算出内容的md5值,使用签名算法签名）
	std::string  strMsgContent;   	//  消息内容
};


//// 消息确认
struct PT_CMD_MESSAGE_CONTENT_ACK {
	PT_CMD_MESSAGE_CONTENT_ACK ()  {
		//this->ver = ADVERTISE_VERSION;
		//this->cmd = CMD_MESSAGE_CONTENT_ACK;
		//this->encrypt = E_None;
	}
	
	friend net_archive& operator << (net_archive& ar, PT_CMD_MESSAGE_CONTENT_ACK& pt) {
		//ar << *(COMMAND_HEADER*) &pt;
	
		ar << pt.uMsgID; 
		ar << pt.emOpResult; 
		return ar;
	}
	
	friend net_archive& operator >> (net_archive& ar, PT_CMD_MESSAGE_CONTENT_ACK& pt) {
		//ar >> *(COMMAND_HEADER*) &pt;
		
		ar >> pt.uMsgID; 
		ar >> pt.emOpResult; 
		return ar;
	}
	
	
	UINT64  uMsgID;   	//  要确认的消息ID
	UINT  emOpResult;   	//  显示结果
};


// #pragma pack()

#endif
