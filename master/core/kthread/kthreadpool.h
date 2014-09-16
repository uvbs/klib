#ifndef _Klib_THREADPOOL_H_
#define _Klib_THREADPOOL_H_

#if !defined(_INC_PROCESS)
#include <PROCESS.H>
#endif

#include <list>
#include "event.h"
#include "auto_lock.h"
#include "../pattern/lock_stl.h"
#include "../macro.h"


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
        task_info(const func_type& f) : f_(f)  { }
        
        void operator ()()  {  this->f_();  }

        func_type f_;
    };

    struct thead_contex 
    {
        thead_contex() : pool_(nullptr), handle_(0) {}

        kthread_pool* pool_;
        HANDLE        handle_;
    };

public:
    kthread_pool(int iThreadNum = 3) ;
    ~kthread_pool();

    ///< 调整线程个数
    bool adjust_thread(int iNumChange) ;

    ///< 停止线程池
    bool stop() ;

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
    inline task_info* fetech_task() ;

    ///< 工作线程
    static unsigned int WINAPI work_thread(void*param) ;

    ///< 线程函数
    void worker() ;
    void remove_thread_handle(HANDLE hthread);

protected:
    bool        stop_;
    LONG        thread_count_;
    LONG        running_thread_count_;           ///< 运行的线程个数
    auto_cs     thread_num_cs_;
    
    Event event_;
    klib::pattern::lock_list<task_info*> tasklist_;
    std::vector<HANDLE> thread_handles_;
};



}}


#endif //_Klib_THREADPOOL_H_