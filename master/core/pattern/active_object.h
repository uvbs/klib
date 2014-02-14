#ifndef _active_object_h_
#define _active_object_h_

#include "../kthread/thread.h"
#include "multi_buffer_queue.h"

namespace klib {
namespace pattern {


/* simple active object pattern */
template<typename msg_type>
class active_object
{
protected:
    /* worker function */
    void work_fun(void*)  
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
        thread_.start(std::bind(&self_type::work_fun, this, std::placeholders::_1), NULL);
    }

    /* stop object */
    void stop()  {   thread_.exit(true);   }

    /* send message */
    bool send(const msg_type& t) 
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



}}












#endif // _active_object_h_

