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


///< 任务接口
class ITask 
{
public:
	virtual BOOL Run(void* param) = 0;
};

///< 线程池
class KThreadPool 
{
    ///< 任务信息，内部使用
    struct TaskInfo 
    {
        TaskInfo(ITask* task, void* param) 
        {
            this->task = task;
            this->param = param;
        }
        ITask* task;
        void* param;
    };

public:
    KThreadPool(int iThreadNum = 3) : m_bStop(FALSE) 
    {
        m_uRunningThradNum = 0;
        iThreadNum = iThreadNum < 2 ? 2 : iThreadNum;
        m_uThreadNum = 0;
        AdjustThread(iThreadNum);
    }

    ~KThreadPool()
    {
        tasklist_.for_each( [&](TaskInfo* p)->bool
        {
            delete p;
            return true;
        });
    }

    ///< 调整线程个数
    BOOL AdjustThread(int iNumChange) 
    {
        auto_lock locker(thread_num_cs_);
        if (iNumChange < 0)  
        {
            if (abs(iNumChange) <= m_uThreadNum)
            {
                m_uThreadNum = m_uThreadNum - abs(iNumChange);
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
                hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThrad, this, 0, NULL);
                CloseHandle(hThread);

            }
            m_uThreadNum += iNumChange;
        }

        return TRUE;
    }

    ///< 停止线程池
    BOOL Stop() 
    {
        m_bStop = TRUE;
        return TRUE;
    }

    ///< 添加一个任务
    inline BOOL AddTask(ITask* task, void* param) 
    {
        event_.signal();

        return tasklist_.push_item(new TaskInfo(task, param));
    }

    ///< 获得线程个数
    inline UINT GetThreadNum() {	return m_uThreadNum; }

    ///< 获得运行中的线程个数
    inline UINT GetRunningThreadNum() { return m_uRunningThradNum; }

protected:
    ///< 获取一个任务
    inline TaskInfo* GetTask() 
    {
        TaskInfo* p = NULL;
        if (tasklist_.pop_item(p))
            return p;

        return NULL;
    }

    ///< 工作线程
    static unsigned int WINAPI WorkThrad(void*param) 
    {
        KThreadPool* theadpool = (KThreadPool*) param;
        _ASSERT(theadpool);

        theadpool->ThreadWorker();

        return 0;
    }

    ///< 线程函数
    void ThreadWorker() 
    {
        //InterlockedIncrement(&m_uRunningThradNum);
        {
            auto_lock locker(thread_num_cs_);
            ++ m_uRunningThradNum;
        }

        TaskInfo* taskinfo = NULL;
        while (!m_bStop) 
        {
            // 取job并执行
            taskinfo = this->GetTask();
            if (NULL == taskinfo) 
            {
                event_.wait(200);

                auto_lock lock(thread_num_cs_);
                if (m_uRunningThradNum > m_uThreadNum) 
                {
                    //运行的线程数较多，需要退出
                    break;
                }
            }
            else 
            {
                _ASSERT(taskinfo->task);
                taskinfo->task->Run(taskinfo->param);
                delete taskinfo;
            }
        }

        {
            auto_lock locker(thread_num_cs_);
            printf("Exit thread :%d\r\n", m_uRunningThradNum);
            --m_uRunningThradNum;
        }

        //InterlockedDecrement(&m_uRunningThradNum);
    }

protected:
    BOOL  m_bStop;
    LONG  m_uThreadNum;
    LONG  m_uRunningThradNum;           ///< 运行的线程个数
    auto_cs  thread_num_cs_;
    
    Event event_;
    klib::stl::CLockList<TaskInfo*> tasklist_;
};



}}


#endif //_Klib_THREADPOOL_H_