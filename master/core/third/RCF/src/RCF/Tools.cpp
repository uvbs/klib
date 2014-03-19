
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

#include <RCF/Tools.hpp>

#include <RCF/util/InitDeinit.hpp>

namespace RCF {

    util::TraceChannel &getRcfTraceChannel()
    {
        static util::TraceChannel rcfTraceChannel("RCF");
        return rcfTraceChannel;
    }

    UTIL_ON_INIT( getRcfTraceChannel() )

}


