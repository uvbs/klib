// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: 在此处引用程序需要的其他头文件
#include <WinSock2.h>
#include <windows.h>


#include <comp/MyNet/imynetdef.h>
#include <core/singleton.h>
#include <pattern/object_pool.h>
using namespace klib::core;

#include <klib_link.h>
#include <rdebug/logger.h>

#include "../../../core/comp/mem_check/library/src/mem_lib.h"
#include "../../../core/comp/mem_check/library/src/redefine_new.h"