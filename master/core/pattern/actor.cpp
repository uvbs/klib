#include "actor.h"
#include "../core/scope_guard.h"

using namespace klib::kthread;
using namespace klib::core;

namespace klib {
namespace pattern{
namespace actor{


engine::~engine() 
{
    pool_.stop();
}

bool engine::init()
{
    auto lp_tsk = std::bind(&engine::loop_task, this);
    pool_.add_task(lp_tsk);

    return true;
}

bool engine::regist(actor_base* actor) 
{
    if (act_list_.find_item(actor))
        return false;

    return act_list_.push_item(actor);
}

bool engine::stop()
{
    pool_.stop();
    return true;
}

void engine::add_task(actor_base* act, size_t num)
{
    kthread_pool::func_type func = std::bind(&engine::actor_task, this, act, num);

    pool_.add_task(func);
};

void engine::engine_loop()
{
    size_t exec_count = 0;
    bool need_sleep = true;

    while (true)
    {
        need_sleep = true;
        auto f = [&](actor_base* act) -> bool
        {
            if (act->is_queued() || act->msg_count() == 0) 
                return true;

            exec_count = act->msg_count() > 10 ? 10 : act->msg_count();
            add_task(act, exec_count);

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

void engine::loop_task()
{
    this->engine_loop();
};

void engine::actor_task(actor_base* act, size_t exec_num)
{
    act->set_queued(true);
    ON_SCOPE_EXIT( act->set_queued(false); );

    size_t counter = 0;
    while (counter < exec_num)
    {
        act->handle();
        ++ counter;
    }

    while (pool_.get_idle_thread_count() > 0 && act->handle()) ;
}

}}}


