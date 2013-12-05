
#include "timer.h"


namespace klib{
namespace core{


timer_mgr::timer_mgr() : timer_id_(0)
{

}

timer_mgr::~timer_mgr()
{
    auto_lock lock(m_sec);

    for (auto itr = timer_heap_.begin(); itr != timer_heap_.end(); ++itr) 
    {
        delete *itr;
    }
    timer_heap_.clear();
}

timer* timer_mgr::add(UINT64 uSpan, timer_func_type fun)
{
    auto_lock  lock(m_sec);

    timer * tmr = NULL;

    tmr == NULL ? tmr = new timer : (void)0;
    if (NULL == tmr)
        return false;

    _ASSERT(uSpan);

    tmr->fun = fun;
    tmr->utimer_id = ++ timer_id_;
    tmr->uSpan = uSpan;

    tmr->wakeup_time = lastTick_ + tmr->uSpan;

    timer_heap_.insert(tmr);

    return tmr;
}

bool timer_mgr::remove(timer* tmr)
{
    auto_lock  lock(m_sec);

    // lazy delete
    tmr->bdelete = true;

    return true;
}

bool timer_mgr::start()
{
    lastTick_ = 0;

    Thread::thread_func_type f = std::bind(&timer_mgr::worker, this, std::tr1::placeholders::_1);
    return thread_.start(f, NULL);
}

void timer_mgr::stop()
{
    thread_.exit(true);
    thread_.wait();
}

void timer_mgr::worker(void* p)
{
    AutoFinalizerHandle<HANDLE> handle_;
    HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    handle_.Attach(handle_);

    while (thread_.state() != Thread::TS_EXIT)
    {
        DWORD dwRet = WaitForSingleObject(hEvent, 100);
        _ASSERT (dwRet == WAIT_TIMEOUT) ;

        if (timer_heap_.empty())
            continue;

        lastTick_ += 100;
        do {
            auto_lock  lock(m_sec);
            timer* tmr = * timer_heap_.begin();

            // 删除延迟删除的定时器
            if (tmr->bdelete) 
            {
                timer_heap_.erase(timer_heap_.begin());
                delete tmr;
                continue;
            }
            
            // 判断需要被唤醒的时间是否是到达
            if (tmr->wakeup_time <= lastTick_) 
            {
                timer_heap_.erase(timer_heap_.begin());

                if (tmr->fun())
                {
                    #ifdef _DEBUG
                    ++ tmr->invoke_times;
                    #endif

                    tmr->wakeup_time += tmr->uSpan;
                    timer_heap_.insert(tmr);
                }
                else
                {
                    delete tmr;
                }
            }
            else
            {
                break;
            }

        } while (true);
    }
}

size_t timer_mgr::size() const
{
    return timer_heap_.size();
}

}}

