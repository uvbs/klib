#pragma once

#include "tcp_net_facade.h"
#include "INetwork.h"
#include "inet_tcp_handler.h"
#include "net_conn.h"
#include "inetpacket_mgr.h"
#include "inet_conn_mgr.h"
#include "icombiner.h"

// 定义接口

// {9B9F6A14-10A1-4F9D-8842-829A93CC15DC}
static const GUID IID_imy_net = 
{ 0x9b9f6a14, 0x10a1, 0x4f9d, { 0x88, 0x42, 0x82, 0x9a, 0x93, 0xcc, 0x15, 0xdc } };

struct imy_net
{
    /// @brief
    /// 创建一个TCP客户操作接口
    virtual tcp_net_facade* create_tcp_facade() = 0;
};