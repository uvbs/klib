
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

#include <RCF/InitDeinit.hpp>

namespace RCF {

    class RcfInitDeinit
    {
    public:
        RcfInitDeinit()
        {
            init();
        }
        ~RcfInitDeinit()
        {
            deinit();
        }
    };

    // In some situations, Win32 DLL's in particular, the user needs to be able to explicitly init and deinit the framework.
    // To do that, define RCF_NO_AUTO_INIT_DEINIT and then manually call RCF::init() and RCF::deinit().

#ifndef RCF_NO_AUTO_INIT_DEINIT

    static RcfInitDeinit rcfInitDeinit;

#endif

} // namespace RCF

