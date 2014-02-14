#ifndef _actor_h_
#define _actor_h_


#include "multi_buffer_queue.h"
#include "../kthread/kthreadpool.h"
#include "../core/lock_stl.h"


namespace klib {
namespace pattern{
namespace actor {

namespace detail
{
    /* internal use of message queue */
    template<typename msg_type>
    class msg_queue
    {
    public:
        typedef msg_type value_type;

    public:
        size_t size() { return queue_.size(); }
        bool pop(msg_type& t) { return queue_.pop(t); }
        bool push(const msg_type& t) { return queue_.push(t); };

    protected:
        multi_buffer_queue<1000000, 5, msg_type> queue_;
    };
}

/* actor base class  */
class engine;
class actor_base
{
    friend engine;
public:
    virtual bool handle() = 0;
    virtual size_t msg_count() = 0;

protected:
    virtual bool is_queued() = 0;
    virtual void set_queued(bool isqueue) = 0;
};

/* actor engine framework */
class engine
{
    struct loop_task;
    struct actor_task;

public:
    ~engine() ;

    bool init() ;
    bool regist(actor_base* actor) ;
    void exec();

protected:
    klib::stl::lock_list<actor_base*> act_list_;
    klib::kthread::kthread_pool pool_;
};

/* actor imp class */
template<class subclass, typename msg_type>
class actor_imp : public actor_base
{
protected:
    virtual bool handle() 
    {
        if (mq_.size() == 0) 
            return false;

        msg_queue_type::value_type t;
        if (mq_.pop(t)) 
        {
            this->execute(t);
            return true;
        }

        return false;
    };

    size_t msg_count()  {   return mq_.size();   }
    bool is_queued()    {   return queued_;      }
    void set_queued(bool flag)  {  queued_ = flag;   }

    /* to implement */
    virtual void execute(msg_type& t) = 0;

public:
    actor_imp (engine& fr) : queued_(false)
    {
        fr.regist(this);
    }
    
    void send(const msg_type& t) {   mq_.push(t);    }

protected:
    typedef detail::msg_queue<msg_type> msg_queue_type;
    msg_queue_type  mq_;
    bool            queued_ ;
};


}}}


#endif