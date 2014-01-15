#pragma once

#include "comp/MyNet/INetClient.h"
#include "comp/MyNet/INetwork.h"
#include "comp/MyNet/INetEventHandler.h"
#include "comp/MyNet/INetConnection.h"
#include "comp/MyNet/INetPacketMgr.h"
#include "comp/MyNet/INetConnectionMgr.h"
#include "comp/MyNet/ICombiner.h"

// 定义接口

// {9B9F6A14-10A1-4F9D-8842-829A93CC15DC}
static const GUID IID_IMyNet = 
{ 0x9b9f6a14, 0x10a1, 0x4f9d, { 0x88, 0x42, 0x82, 0x9a, 0x93, 0xcc, 0x15, 0xdc } };

struct IMyNet
{
    /// @brief
    /// 创建一个TCP客户操作接口
    virtual INetTcpClient* CreateTcpClient() = 0;
};