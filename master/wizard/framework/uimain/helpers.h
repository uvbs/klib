#pragma once

#include <core/singleton.h>
#include <core/lock_stl.h>

#include "..\common\ifchatdef.h"

using namespace klib::core;
using namespace klib::stl;
using namespace klib::kthread;

// 插件的列表
class CPlugList : 
    public lock_list<IPlug*>,
    public singleton<CPlugList>
{
public:
};

// 任务管理器
class CTaskMgr : 
    public singleton<CTaskMgr>,
    public klib::core::task_mgr
{
    DECLARE_SINGLETON_CLASS(CTaskMgr);

};
