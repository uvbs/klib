#pragma once

#include <functional>

#include "singleton.h"
#include "klib_core.h"
#include "lock_stl.h"
#include "event.h"
#include "../kthread/thread.h"


namespace klib {
namespace core {


//----------------------------------------------------------------------
// 任务管理器 
class task_mgr
{
public:
    typedef std::function<void()> task_func_type;

public:
    task_mgr();

    bool add_task(const task_func_type& fun);

    void start();
    void stop();

    size_t size() const;

protected:
    void worker(void* param);

protected:
    klib::kthread::Thread                   thread_;
    klib::kthread::Event                    event_;
    klib::stl::CLockList<task_func_type>    task_list_;
};



}}
