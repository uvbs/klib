#ifndef _klib_act_list_h_
#define _klib_act_list_h_

#include <list>
#include <functional>

typedef std::function<void(void)> act_func_type;


#include "../kthread/mutex.h"
#include "../kthread/auto_lock.h"

namespace klib{
namespace pattern{


class act_list
{
public:
    bool add_act(act_func_type func);
    bool add_act(act_func_type&& func);

    // execute by another thread
    bool exec();
protected:
    klib::kthread::mutex      mutex_;
    std::list<act_func_type>  act_lst_;
};


bool act_list::add_act(act_func_type func)
{
    klib::kthread::guard guard_(mutex_);
    act_lst_.push_back(func);
    return true;
}

bool act_list::add_act(act_func_type&& func)
{
    klib::kthread::guard guard_(mutex_);
    act_lst_.push_back(std::move(func));
    return true;
}

bool act_list::exec()
{
    if (act_lst_.empty()) {
        return false;
    }

    act_func_type func;
    klib::kthread::lock_exec(mutex_, [&]()
    {
        func = std::move(act_lst_.front());
        act_lst_.pop_front();
    }
    );

    if (func)
        func();

    return true;
}


}}



#endif // _klib_act_list_h_