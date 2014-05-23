
#ifndef _fiber_scheduler_h_
#define _fiber_scheduler_h_

#include "coroutine.h"
#include <kthread/kthreadpool.h>

namespace fiber
{



class scheduler_basic 
{
public:
    typedef coroutine_basic<scheduler_basic> coroutine_type;
    typedef std::shared_ptr<coroutine_type> coroutine_pointer;

public:
    void run()
    {
    }

    void stop()
    {
        
    }

    template <typename F>
    void spawn(const F& f, std::size_t stack_size = 0) 
    {
        coroutine_pointer co(new coroutine_type(*this, f, stack_size));
        all_.insert(co);

        pool_.add_task(std::bind(&scheduler_basic::run, this));
    }

protected:

private:
    std::set<coroutine_pointer> all_;
    klib::pattern::actor::engine  engine_;

    kthread_pool pool_;
};



}

#endif // _fiber_scheduler_h_