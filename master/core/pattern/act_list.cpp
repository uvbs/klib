#include "act_list.h"

using namespace klib::pattern;

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
#include "act_list.h"


bool act_list::exec()
{
    if (act_lst_.empty()) {
        return false;
    }

    act_func_type func;
    klib::kthread::lock_exec(mutex_, [&]()
        {
            if (!act_lst_.empty()) 
            {
                func = std::move(act_lst_.front());
                act_lst_.pop_front();
            }
        }
    );

    if (func)
        func();

    return true;
}