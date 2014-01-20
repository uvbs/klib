#ifndef _klib_thread_local_h_
#define _klib_thread_local_h_


#include <windows.h>

namespace klib {
namespace kthread {


// 线程本地存储
template<typename t_host_type>
class thread_local
{
public:
    typedef thread_local<t_host_type>  self_type;

    thread_local()
    {
        tls_index_ = TlsAlloc();
    }
    
    t_host_type* get()
    {
        t_host_type* v = TlsGetValue(tls_index_);
        if (NULL == v) {
            v = new t_host_type;
            TlsSetValue(tls_index_, v);
        }

        return v;
    }

protected:
    DWORD  tls_index_;
};



}}






#endif // _klib_thread_local_h_