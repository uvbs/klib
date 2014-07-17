#include "perf_tool.h"

using namespace klib::util;
using namespace klib::kthread;


perf_mgr::~perf_mgr()
{
}

// 启动线程，创建文件
int perf_mgr::start(const std::string& path_, int seconds_)
{
    Thread::thread_func_type func = std::bind(&perf_mgr::worker, this);
    thread_.start(func);
    return 0;
}

// 关闭线程
int perf_mgr::stop()
{
    thread_.exit();
    thread_.wait();
    return 0;
}

// 增加性能监控数据
void perf_mgr::post(const std::string& mod_, long us)
{
    perf_item_t itm;
    itm.mod = mod_;
    itm.cost = us;
    buffer_.push(std::move(itm));
}

void perf_mgr::worker()
{
    while (thread_.state() != Thread::TS_EXIT)
    {
        perf_item_t t;
        while (buffer_.pop(t))
        {
            
        }

        Sleep(200);
    }
}



