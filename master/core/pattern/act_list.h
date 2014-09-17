#ifndef _klib_act_list_h_
#define _klib_act_list_h_

#include <list>
#include <functional>



#include "../kthread/mutex.h"
#include "../kthread/auto_lock.h"

namespace klib{
namespace pattern{

typedef std::function<void(void)> act_func_type;

class act_list
{
public:
    // add for a thread
    bool async_act(act_func_type& func);
    bool sync_act(act_func_type& func);

    // execute by another thread
    bool exec();
protected:
    klib::kthread::mutex      mutex_;
    std::list<act_func_type*>  act_lst_;
};



}}



#endif // _klib_act_list_h_