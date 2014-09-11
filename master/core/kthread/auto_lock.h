
#if !defined(_Klib_AutoLock_H)
#define _Klib_AutoLock_H

#include "mutex.h"

namespace klib {
namespace kthread {

typedef guard  auto_lock ;
typedef mutex  auto_cs   ;

// scope lock
template<typename T, typename F> void lock_exec( T& t, F f ) { guard hold(t); f(); }

#define  LOCK_EXEC(t, act)  \
    lock_exec(t, [&]() { act; });

}}


#endif //