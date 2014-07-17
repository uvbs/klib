#ifndef _active_object_h_
#define _active_object_h_

#include "../kthread/event.h"
#include "../kthread/thread.h"
#include "multi_buffer_queue.h"

using namespace ::klib::kthread;

namespace klib {
namespace pattern {


/* simple active object pattern */
template<typename msg_type>
class active_object
{
protected:
    /* worker function */
    void work_fun()  
    {
        msg_type t;

        while (thread_.state() != Thread::TS_EXIT)
        {
            while (queue_.pop(t)) 
            {
                this->execute(t);
            }

            evt_.wait(300);
        }
    }

    /* really execute func */
    virtual bool execute(msg_type& t) = 0;

public:
    typedef active_object<msg_type> self_type;

public:
    ~active_object()
    {
        this->stop();
    }

    /* start object */
    void start()
    {
        thread_.start(std::bind(&self_type::work_fun, this));
    }

    /* stop object */
    void stop()  
    {
        thread_.exit(true);
        thread_.wait();
    }

    /* send message */
    bool send(msg_type t) 
    { 
        auto ret = queue_.push(t);
        if (queue_.size() <= 1) {
            evt_.signal();
        }

        return ret;
    }

    size_t size() { return queue_.size(); }

protected:
    kthread::Thread     thread_;            // working thread
    win_event evt_;
    multi_buffer_queue<1000000, 4, msg_type> queue_;        // msg queue
};

template<typename obj_type>
class active_obj_mgr
{
public:
    ~active_obj_mgr()
    {
        if (NULL == worker_arr_) {
            return;
        }

        for (size_t index=0; index<worker_num_; ++index)
        {
            (worker_arr_ + index)->stop();
        }
        delete [] worker_arr_;
    }

    bool init(size_t worker_num = 1)
    {
        worker_arr_ = new obj_type[worker_num];
        if (NULL == worker_arr_) {
            return false;
        }

        for (size_t index=0; index<worker_num; ++index)
        {
            (worker_arr_ + index)->start();
        }

        worker_num_ = worker_num;
        return true;
    }

    obj_type* choose_worker(void* p)
    {
        return worker_arr_ + ((size_t)p % worker_num_);
    }

protected:
    obj_type*   worker_arr_;
    size_t      worker_num_;
};


}}












#endif // _active_object_h_

