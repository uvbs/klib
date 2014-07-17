#ifndef _actor_h_
#define _actor_h_


#include "../kthread/thread.h"
#include "../kthread/thread_local.h"
#include "../kthread/kthreadpool.h"
#include "../core/lock_stl.h"

#include "multi_buffer_queue.h"


namespace klib {
namespace pattern{
namespace actor {

namespace detail
{
    /* internal use of message queue, local_store_num 本地存储的个数 */
    template<typename msg_type, int local_store_num = 100>
    class msg_queue
    {
    public:
        typedef msg_type value_type;
        typedef klib::kthread::thread_local<std::list<msg_type>> local_msg_list_type;

    public:
        size_t size() { return queue_.size(); }
        bool pop(msg_type& t) { return queue_.pop(t); }
        
        bool push_now(const msg_type& t) 
        {
            local_msg_list_type::value_type_ptr v = local_lst_.get();
            if (!v->empty())
                sync();

            return queue_.push(t); 
        }

        bool push(const msg_type& t) 
        {
            local_msg_list_type::value_type_ptr v = local_lst_.get();
            v->push_back(t);
            if (v->size() >= local_store_num) 
            {
                queue_.push(*v); 
                v->clear();
            }
            return true;
        };
        void sync()
        {
            local_msg_list_type::value_type_ptr v = local_lst_.get();
            if (!v->empty())
            {
                queue_.push(*v); 
                v->clear();
            }
            return ;
        }

    protected:
        local_msg_list_type local_lst_;
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
    engine();
    ~engine() ;

    bool init() ;
    bool regist(actor_base* actor) ;
    bool stop();

protected:
    void engine_loop();
    void add_task(actor_base* act, size_t num);
    void actor_task(actor_base* act, size_t exec_num);

protected:
    klib::stl::lock_list<actor_base*> act_list_;
    klib::kthread::kthread_pool thread_pool_;
    klib::kthread::Thread       work_thread_;
    bool                        is_stop_;
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
    actor_imp () : queued_(false)
    {
    }
    
    void send(const msg_type& t) { mq_.push(t); }
    void sync() { mq_.sync(); }

protected:
    typedef detail::msg_queue<msg_type> msg_queue_type;
    msg_queue_type  mq_;
    bool            queued_ ;
};


}}}


#endif