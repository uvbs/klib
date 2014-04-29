#ifndef _Klib_THREADPOOL_H_
#define _Klib_THREADPOOL_H_

#if !defined(_INC_PROCESS)
#include <PROCESS.H>
#endif

#include <list>
#include "auto_lock.h"
#include "../core/lock_stl.h"
#include "event.h"


namespace klib{
namespace kthread {

class kthread_pool;


///< 线程池
class kthread_pool 
{
public:
    typedef std::function<void()> func_type;

    ///< 任务信息，内部使用
    struct task_info 
    {
        task_info(const func_type& f) : f_(f)
        {
        }
        
        void operator ()()
        {
            this->f_();
        }

        func_type f_;
    };

public:
    kthread_pool(int iThreadNum = 3) : stop_(FALSE) 
    {
        running_thread_count_ = 0;
        iThreadNum = iThreadNum < 2 ? 2 : iThreadNum;
        thread_count_ = 0;
        adjust_thread(iThreadNum);
    }

    ~kthread_pool()
    {
        tasklist_.for_each( [&](task_info* p)->bool
        {
            delete p;
            return true;
        });
    }

    ///< 调整线程个数
    bool adjust_thread(int iNumChange) 
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
                hThread = (HANDLE)_beginthreadex(NULL, 0, work_thread, this, 0, NULL);
                CloseHandle(hThread);

            }
            thread_count_ += iNumChange;
        }

        return TRUE;
    }

    ///< 停止线程池
    bool stop() 
    {
        stop_ = TRUE;
        return TRUE;
    }

    ///< 添加一个任务
    inline bool add_task(const func_type& func)
    {
        event_.signal();

        return tasklist_.push_item(new task_info(func));
    }

    ///< 获得线程个数
    inline UINT get_thread_count() {	return thread_count_; }

    ///< 获得运行中的线程个数
    inline UINT get_running_thread_count() { return running_thread_count_; }

    ///< 获取空闲的线程数
    inline UINT get_idle_thread_count() { return thread_count_ - running_thread_count_; }

protected:
    ///< 获取一个任务
    inline task_info* fetech_task() 
    {
        task_info* p = NULL;
        if (tasklist_.pop_item(p))
            return p;

        return NULL;
    }

    ///< 工作线程
    static unsigned int WINAPI work_thread(void*param) 
    {
        kthread_pool* theadpool = (kthread_pool*) param;
        _ASSERT(theadpool);

        theadpool->worker();

        return 0;
    }

    ///< 线程函数
    void worker() 
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

protected:
    bool        stop_;
    LONG        thread_count_;
    LONG        running_thread_count_;           ///< 运行的线程个数
    auto_cs     thread_num_cs_;
    
    Event event_;
    klib::stl::lock_list<task_info*> tasklist_;
};



}}


#endif //_Klib_THREADPOOL_H_