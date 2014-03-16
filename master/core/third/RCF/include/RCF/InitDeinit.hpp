
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _RCF_INITDEINIT_HPP_
#define _RCF_INITDEINIT_HPP_

#include <RCF/util/InitDeinit.hpp>

#define RCF_ON_INIT(x)                      UTIL_ON_INIT(x)
#define RCF_ON_DEINIT(x)                  UTIL_ON_DEINIT(x)
#define RCF_ON_INIT_DEINIT(x,y)       UTIL_ON_INIT_DEINIT(x,y)

namespace RCF {

    inline void init()
    {
        util::invokeInitCallbacks();
    }

    inline void deinit()
    {
        util::invokeDeinitCallbacks();
    }

} // namespace RCF

#endif // ! _RCF_INITDEINIT_HPP_
