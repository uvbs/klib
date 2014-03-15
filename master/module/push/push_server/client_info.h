#pragma once

using namespace klib::mem;

#include <net/ip_v4.h>
#include <pattern/fix_circle_buff.h>


//----------------------------------------------------------------------
// 客户端确认了的消息ID
struct confirm_info
{
    confirm_info() : uMsgID(0), uConfirmTime(0) {}
    confirm_info(UINT64 uID, UINT64 uTime) : uMsgID(uID), uConfirmTime(uTime) {}

    UINT64  uMsgID;
    UINT64  uConfirmTime;
};


//----------------------------------------------------------------------
// 客户端信息
class client_info
{
public:
    client_info() : 
        client_port_(0),

        login_time_(0), 
        last_active_time_(0), 
        heart_count_(0),
        version_(0),

        last_msg_id_(0)
     {}
    ~client_info(void) {}
    
    ip_v4   client_addr_;               ///< 客户端的IP地址
    USHORT  client_port_;               ///< 客户端的端口（网络字节序）

    UINT64  login_time_;                ///< 登陆的时间
    UINT64  last_active_time_;          ///< 上次活跃的时间
    UINT64  heart_count_;               ///< 心跳个数
    UINT    version_;                   ///< 版本值

    UINT64  last_msg_id_;               ///< 消息的编号
    small_string<13>  mac_;             ///< mac地址，未使用
    small_string<21>  channel_;         ///< 客户端的渠道
    small_string<31>  login_name_;      ///< 电脑登陆名
    small_string<31>  account_;         ///< 用衣的帐号

    UINT64    uid;                      ///< 用户的ID

    fix_circle_buff<confirm_info, UINT8, 10> confirm_lst_;
};

