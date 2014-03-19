//*****************************************************************************
// Copyright (c) 2005. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

#ifndef _UTIL_INITDEINIT_HPP_
#define _UTIL_INITDEINIT_HPP_

#include "AutoRun.hpp"

namespace util {

    typedef void(*InitCallback)();
    typedef void(*DeinitCallback)();

    static const int InitCallbackMaxCount = 25;
    static const int DeinitCallbackMaxCount = 25;

    class InitCallbackArray
    {
    public:
        static InitCallback *get()
        {
            static InitCallback initCallbacks[InitCallbackMaxCount];
            return initCallbacks;
        }
        static bool &getInitCallbacksInvoked()
        {
            static bool initCallbacksInvoked;
            return initCallbacksInvoked;
        }
    };

    class DeinitCallbackArray
    {
    public:
        static DeinitCallback *getStatic()
        {
            static DeinitCallback deinitCallbacks[DeinitCallbackMaxCount];
            return deinitCallbacks;
        }
        static DeinitCallback *&getAllocated()
        {
            static DeinitCallback *deinitCallbacks;
            return deinitCallbacks;
        }
    };

    inline InitCallback *getInitCallbackArray()
    {
        return InitCallbackArray::get();
    }

    inline DeinitCallback *getDeinitCallbackArray()
    {
        return DeinitCallbackArray::getAllocated() ?
            DeinitCallbackArray::getAllocated() :
            DeinitCallbackArray::getStatic();
    }

    inline bool getInitCallbacksInvoked()
    {
        return InitCallbackArray::getInitCallbacksInvoked();
    }

    inline void setInitCallbacksInvoked(bool initCallbacksInvoked)
    {
        InitCallbackArray::getInitCallbacksInvoked() = initCallbacksInvoked;
    }

    template<typename Callback>
    inline bool addCallback(Callback *callbackArray, Callback callback, const int callbacksMaxCount)
    {
        for (int i=0; i<callbacksMaxCount; ++i)
        {
            if (0 == callbackArray[i])
            {
                callbackArray[i] = callback;
                return true;
            }
            else if (callback == callbackArray[i])
            {
                return true;
            }
        }
        return false;
    }

    template<typename Callback>
    inline void invokeCallbacks(Callback *callbackArray,  const int callbacksMaxCount)
    {
        int i=0;
        while (i<callbacksMaxCount && callbackArray[i])
        {
            Callback callback = callbackArray[i];
            (*callback)();
            i++;
        }
    }
    
    // TODO: only following functions should be publicly accessible

    template<typename T>
    inline bool addInitCallback(T (*initCallback)())
    {
        if (getInitCallbacksInvoked())
        {
            initCallback();
        }
        addCallback(getInitCallbackArray(), InitCallback(initCallback), InitCallbackMaxCount);
        return true;
    }

    inline bool addDeinitCallback(DeinitCallback deinitCallback)
    {
        addCallback(getDeinitCallbackArray(), DeinitCallback(deinitCallback), DeinitCallbackMaxCount);
        return true;
    }

    inline void invokeInitCallbacks()
    {
        invokeCallbacks(getInitCallbackArray(), InitCallbackMaxCount);
        setInitCallbacksInvoked(true);
    }

    inline void invokeDeinitCallbacks()
    {
        invokeCallbacks(getDeinitCallbackArray(), DeinitCallbackMaxCount);
        setInitCallbacksInvoked(false);
    }

} // namespace util


// TODO: throughly specify how these macros interact with multiple translation units

#define UTIL_ON_INIT_DEINIT_IMPL(statements, functorName, whichAction)                                             \
class functorName {                                                                                                                               \
public:                                                                                                                                                  \
    functorName() { static bool first = true; if (first) { first = false; whichAction(&functorName::invoke); } }          \
    static void invoke() { statements; }                                                                                                     \
};                                                                                                                                                          \
static functorName BOOST_PP_CAT(functorName, Inst);

#define UTIL_ON_INIT(statements)                            UTIL_ON_INIT_NAMED(statements, selfRegisteringInitFunctor)
#define UTIL_ON_DEINIT(statements)                          UTIL_ON_DEINIT_NAMED(statements, selfRegisteringDeinitFunctor)
#define UTIL_ON_INIT_NAMED(statements, name)                UTIL_ON_INIT_DEINIT_IMPL(statements, BOOST_PP_CAT(name, __LINE__), ::util::addInitCallback )
#define UTIL_ON_DEINIT_NAMED(statements, name)              UTIL_ON_INIT_DEINIT_IMPL(statements, BOOST_PP_CAT(name, __LINE__), ::util::addDeinitCallback )
#define UTIL_ON_INIT_DEINIT(initFunc, deinitFunc)           UTIL_ON_INIT(initFunc) UTIL_ON_DEINIT(deinitFunc)

#endif // ! _UTIL_INITDEINIT_HPP_
