// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>



// TODO: 在此处引用程序需要的其他头文件

#include <aom/iframework.h>
#include <aom/iframework_imp.h>
#include <klib_link.h>

#include <net/udp_client.h>
#include <net/winsock_init.h>
#include <net/winsock_link.h>
#include <net/proto/net_archive.h>
#include <net/proto/command_header.h>

#include <util/stop_watch.h>
#include <core/singleton.h>
#include <debuglog.h>
#include <pattern/fsm.h>
#include <kthread/thread.h>

using namespace klib::core;
using namespace klib::aom;

using namespace klib::net;
using namespace klib::util;
using namespace klib::debug;
using namespace klib::pattern::fsm;
