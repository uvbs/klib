
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

#include <RCF/Random.hpp>

#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <RCF/ThreadLibrary.hpp>
#include <RCF/Tools.hpp>
//#include <RCF/util/AutoRun.hpp>
#include <RCF/util/InitDeinit.hpp>
#include <RCF/util/Platform/Platform.hpp>

namespace RCF {

    int generateRandomNumber()
    {
        static bool first = true;
        if (first)
        {
            struct _timeb timebuffer;
            _ftime( &timebuffer );
            int timeS = static_cast<int>(timebuffer.time);
            int timeMs = timebuffer.millitm;
            srand(timeS ^ timeMs);
            first = false;
        }
        static Platform::Threads::mutex m;
        {
            Platform::Threads::mutex::scoped_lock lock(m); RCF_UNUSED_VARIABLE(lock);
            return rand();
        }
    }

    UTIL_ON_INIT( generateRandomNumber() )

}
