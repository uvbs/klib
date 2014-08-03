#ifndef _klib_timer_mgr_h
#define _klib_timer_mgr_h

#include <windows.h>
#include <set>
#include <map>


#include "../kthread/auto_lock.h"
#include "../kthread/thread.h"
using namespace klib::kthread;


namespace klib {
namespace core {

// 返回false表示需要被删除
// 返回true表示需要继续
typedef std::function<bool(void)> timer_func_type;

// 定时器信息
struct lessfun;
class  timer_mgr;
struct timer
{
    timer() : bdelete(false), utimer_id(0), wakeup_time(0), uSpan(0)
    {
        #ifdef _DEBUG
        invoke_times = 0;
        #endif
    }
    
    friend lessfun;
    friend timer_mgr;

    /// function
    #ifdef _DEBUG
    UINT32 invoked_times() const { return invoke_times; }
    #endif

    UINT32 id() const            { return utimer_id;    }
    
protected:
    bool            bdelete;        // 表示是否删除了
    UINT32          utimer_id;      // ID
    UINT64          uSpan;          // 毫秒
    UINT64          wakeup_time;    // 下次唤醒的时间
    
    timer_func_type    fun;            // fun

    #ifdef _DEBUG
    UINT32          invoke_times;   // 呼叫次数
    #endif
};


// 定时器实现
class timer_mgr
{
public:

public:
    timer_mgr();
    ~timer_mgr();

    timer* add(UINT64 uSpan, timer_func_type fun);
    bool remove(timer* tmr);

    bool start();
    void stop();

    size_t size() const;

protected:
    void worker();

protected:
    struct lessfun
    {
        bool operator()(timer* left, timer* right)
        {
            if (left->wakeup_time < right->wakeup_time)
                return true;

            if (left->wakeup_time > right->wakeup_time)
                return false;

            return left->utimer_id < right->utimer_id;
        }
    };


protected:
    std::set<timer*, lessfun> timer_heap_;

    Thread          thread_;
    UINT64          lastTick_;
    UINT32          timer_id_;
    auto_cs         m_sec;
};

}}


#endif
