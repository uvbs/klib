
#include "task_mgr.h"

#include <process.h>

using namespace klib::kthread;

namespace klib {
namespace core {

task_mgr::task_mgr() : event_(NULL, TRUE)
{
}

bool task_mgr::add_task(const task_func_type& fun)
{
    task_list_.push_item(fun);
    event_.signal();
    return true;
}

void task_mgr::start()
{
    Thread::thread_func_type fun = std::bind(&task_mgr::worker, this);
    thread_.start(fun);
}

void task_mgr::stop()
{
    thread_.exit(true);
    thread_.wait();
}

void task_mgr::worker()
{
    while ( thread_.state() != Thread::TS_EXIT)
    {
        event_.wait(500);

        task_func_type fun;
        while (task_list_.pop_item(fun))
        {
            fun();
        }

        event_.reset();
    }
}

size_t task_mgr::size() const 
{
    return task_list_.size();
}

}}
