#ifndef _klib_network_interface_h_
#define _klib_network_interface_h_


#include <istddef.h>
#include <macro.h>
using namespace klib;


// {9C5E8FB1-A445-40a2-9BE9-E2EAC6AC6067}
static const GUID IID_NETWORK_I = 
{ 0x9c5e8fb1, 0xa445, 0x40a2, { 0x9b, 0xe9, 0xe2, 0xea, 0xc6, 0xac, 0x60, 0x67 } };


// 模块设计按职责来分
/*
包括客户端与服务端接口（避免使用接口）

 客户端类，
 客户端管理类
 服务端类
 监听类
 网络数据处理类
 调度类
 配置类

*/

namespace network
{


}




#endif // _klib_network_interface_h_