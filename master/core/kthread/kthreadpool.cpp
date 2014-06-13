
#include "kthreadpool.h"

using namespace klib::kthread;

kthread_pool::kthread_pool(int iThreadNum/* = 3*/) : stop_(FALSE) 
{
    running_thread_count_ = 0;
    iThreadNum = iThreadNum < 2 ? 2 : iThreadNum;
    thread_count_ = 0;
    adjust_thread(iThreadNum);
}

kthread_pool::~kthread_pool()
{
    tasklist_.for_each( [&](task_info* p)->bool
    {
        delete p;
        return true;
    });
}

bool kthread_pool::adjust_thread(int iNumChange) 
{
    auto_lock locker(thread_num_cs_);
    if (iNumChange < 0)  
    {
        if (abs(iNumChange) <= thread_count_)
        {
            thread_count_ = thread_count_ - abs(iNumChange);
        }
        else 
        {
            return FALSE;
        }
    }
    else 
    {
        HANDLE hThread = NULL;
        for (int i=0; i<iNumChange; ++i) 
        {
            thead_contex* ctx = new thead_contex;
            ctx->pool_ = this;

            hThread = (HANDLE)_beginthreadex(NULL, 
                0, 
                work_thread, 
                ctx, 
                CREATE_SUSPENDED, 
                NULL);
            ctx->handle_ = hThread;
            thread_handles_.push_back(hThread);

            ResumeThread(hThread);
        }
        thread_count_ += iNumChange;
    }

    return TRUE;
}

bool kthread_pool::stop() 
{
    while (tasklist_.size() > 0)
    {
        Sleep(200);
    }

    stop_ = TRUE;

    while (!thread_handles_.empty())
    {
        Sleep(200);
    }
    return TRUE;
}

kthread_pool::task_info* kthread_pool::fetech_task() 
{
    task_info* p = NULL;
    if (tasklist_.pop_item(p))
        return p;

    return NULL;
}

///< 工作线程
unsigned int WINAPI kthread_pool::work_thread(void*param) 
{
    thead_contex* ctx = (thead_contex*) param;
    _ASSERT(ctx);

    ctx->pool_->worker();
    ctx->pool_->remove_thread_handle(ctx->handle_);

    return 0;
}

///< 线程函数
void kthread_pool::worker() 
{
    task_info* taskinfo = NULL;
    while (!stop_) 
    {
        // 取job并执行
        taskinfo = this->fetech_task();
        if (NULL == taskinfo) 
        {
            event_.wait(200);

            auto_lock lock(thread_num_cs_);
            if (running_thread_count_ > thread_count_) 
            {
                break;
            }
        }
        else 
        {
            if (true)
            {
                auto_lock locker(thread_num_cs_);
                ++ running_thread_count_;
            }

            (*taskinfo)();
            delete taskinfo;

            if (true)
            {
                auto_lock locker(thread_num_cs_);
                -- running_thread_count_;
            }
        }
    }
}

void kthread_pool::remove_thread_handle(HANDLE hthread)
{
    auto_lock locker(thread_num_cs_);

    KLIB_ASSERT(hthread != 0);
    for (auto itr = thread_handles_.begin(); 
        itr != thread_handles_.end();
        ++ itr)
    {
        if (*itr == hthread) {
            thread_handles_.erase(itr);
            return;
        }
    }
    KLIB_ASSERT(false);
}

