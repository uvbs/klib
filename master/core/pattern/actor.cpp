#include "actor.h"

namespace klib {
namespace pattern{
namespace actor{

struct engine::loop_task : public itask
{
    bool execute(kthread_pool* pool, void* param) 
    {
        engine* fr = (engine*) param;
        fr->exec();
        
        return true; 
    }
};

struct engine::actor_task : public itask
{
public:
    actor_task(actor_base* act, size_t exec_num) : actor_(act), exec_num_(exec_num)
    {
        act->set_queued(true);
    }

    bool execute(kthread_pool* pool, void* param) 
    {
        size_t counter = 0;
        while (counter < exec_num_)
        {
            actor_->handle();
            ++ counter;
        }

        while (pool->get_idle_thread_count() > 0 && actor_->handle()) ;
        
        actor_->set_queued(false);

        return true; 
    }

protected:
    actor_base* actor_;
    size_t exec_num_;
};

engine::~engine() 
{
    pool_.stop();
    
}

bool engine::init()
{
    loop_task* task_ = new loop_task;
    
    pool_.add_task(task_, this);

    return true;
}

bool engine::regist(actor_base* actor) 
{
    if (act_list_.find_item(actor))
        return false;

    return act_list_.push_item(actor);
}

void engine::exec()
{
    size_t exec_count = 0;
    bool need_sleep = true;

    while (true)
    {
        need_sleep = true;
        auto f = [&](actor_base* act) -> bool
        {
            if (act->is_queued() || act->msg_count() == 0) 
            {
                return true;
            }

            exec_count = act->msg_count() > 10 ? 10 : act->msg_count();
            actor_task* task_ = new actor_task(act, exec_count);
            pool_.add_task(task_, this);

            need_sleep = false;

            return true;
        };
        act_list_.for_each(f);

        if (need_sleep) 
        {
            Sleep(1000);
        }
    }

}



}}}


