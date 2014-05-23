#ifndef _fiber_coroutine_h
#define _fiber_coroutine_h



namespace fiber
{

typedef std::function<void()> func_type;

struct win_fiber
{
public:
    win_fiber(func_type f, std::size_t stack_size = 0) : 
         f_(f)
    {
        addr_ = CreateFiber(stack_size, fiber_proxy, this);
    }

    void operator()()
    {
        this->f_();
    }

    static VOID CALLBACK fiber_proxy(PVOID lpParameter)
    {
        win_fiber* p = (win_fiber*) lpParameter;
        p->f_();
    }

protected:
    func_type f_;
    void* addr_;
};


template<typename _scheduler_type>
class coroutine_basic : 
    public std::enable_shared_from_this<coroutine_basic<_scheduler_type> >
{
public:
    typedef coroutine_basic self_type;

public:
    coroutine_basic(_scheduler_type& sche, const func_type& f, std::size_t stack_size) : 
        scheduler_(sche),
        fiber_(f, stack_size)
    {
    }

    void yield() {
        // SwitchToFiber();
    }

    void resume() {
    }

    void sleep() {
    }

private:
    win_fiber fiber_;
    _scheduler_type& scheduler_;
};


}



#endif // _fiber_coroutine_h