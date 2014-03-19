
//*****************************************************************************
// Copyright (c) 2003. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

//*****************************
// DEPRECATED!!!
//*****************************

#ifndef _UTIL_AUTORUN_HPP_
#define _UTIL_AUTORUN_HPP_

#define AUTO_RUN(func)                                _AUTO_RUN(func,  __LINE__) 
#define _AUTO_RUN(func, nID)                        __AUTO_RUN(func, nID)
#define __AUTO_RUN(func, nID)                        ___AUTO_RUN(func, nID)
#define ___AUTO_RUN(func, ID)                                                           \
    namespace {                                                                                  \
        struct AutoRun##ID {                                                                  \
            AutoRun##ID() {                                                                      \
                func;                                                                                 \
            }                                                                                           \
        } AutoRunInst##ID;                                                                     \
    }

    // More concise to implement AUTO_RUN using the following, but BCB emits an ICE on it.
    //static bool AutoRun##ID = ( func , false);


#define AUTO_RUN_1(func)                            AUTO_RUN_NAMED(func, 1)                    
#define AUTO_RUN_2(func)                            AUTO_RUN_NAMED(func, 2)                    
#define AUTO_RUN_3(func)                            AUTO_RUN_NAMED(func, 3)                    
#define AUTO_RUN_4(func)                            AUTO_RUN_NAMED(func, 4)                    
#define AUTO_RUN_5(func)                            AUTO_RUN_NAMED(func, 5)                    

#define AUTO_RUN_NAMED(func, name)                    _AUTO_RUN_NAMED(func, name, __LINE__) 
#define _AUTO_RUN_NAMED(func, name, nID)            __AUTO_RUN_NAMED(func, name, nID)
#define __AUTO_RUN_NAMED(func, name, nID)            ___AUTO_RUN(func, _##name##_##nID)

#define AUTO_RUN_ONCE(func)                            _AUTO_RUN_ONCE(func,  __LINE__) 
#define _AUTO_RUN_ONCE(func, nID)                    __AUTO_RUN_ONCE(func, nID)
#define __AUTO_RUN_ONCE(func, nID)                    ___AUTO_RUN_ONCE(func, nID)
#define ___AUTO_RUN_ONCE(func, ID)                                                         \
    struct AutoRunOnce##ID {                                                                      \
        AutoRunOnce##ID() {                                                                          \
            if (!init()) {                                                                                     \
                init() = true;                                                                              \
                func;                                                                                         \
            }                                                                                                   \
        }                                                                                                       \
        static bool &init() {                                                                            \
            static bool bInit = false;                                                                 \
            return bInit;                                                                                  \
        }                                                                                                       \
    };                                                                                                          \
    static AutoRunOnce##ID AutoRunOnceInst##ID;

#define AUTO_RUN_ONCE_1(func)                            AUTO_RUN_ONCE_NAMED(func, 1)                    
#define AUTO_RUN_ONCE_2(func)                            AUTO_RUN_ONCE_NAMED(func, 2)                    
#define AUTO_RUN_ONCE_3(func)                            AUTO_RUN_ONCE_NAMED(func, 3)                    
#define AUTO_RUN_ONCE_4(func)                            AUTO_RUN_ONCE_NAMED(func, 4)                    
#define AUTO_RUN_ONCE_5(func)                            AUTO_RUN_ONCE_NAMED(func, 5)                    

#define AUTO_RUN_ONCE_NAMED(func, name)                    _AUTO_RUN_ONCE_NAMED(func, name, __LINE__) 
#define _AUTO_RUN_ONCE_NAMED(func, name, nID)            __AUTO_RUN_ONCE_NAMED(func, name, nID)
#define __AUTO_RUN_ONCE_NAMED(func, name, nID)            ___AUTO_RUN_ONCE(func, _##name##_##nID)

#endif // ! _UTIL_AUTORUN_HPP_
