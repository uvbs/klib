#pragma once

using namespace klib::mem;

#include <net/ip_v4.h>
#include <pattern/fix_circle_buff.h>

#include "../../include/push_interface.h"

namespace logic
{


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
class client_info_ex : public client_info
{
public:
    client_info_ex() 
     {}
    ~client_info_ex(void) {}

    fix_circle_buff<confirm_info, UINT8, 10> confirm_lst_;
};



}
