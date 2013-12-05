#pragma once

namespace klib {
namespace debug {

#ifdef _DEBUG

    #define DEBUG_CLIENTBLOCK    new( _CLIENT_BLOCK, __FILE__, __LINE__)

#else

    #define DEBUG_CLIENTBLOCK

#endif



#ifdef _DEBUG

    #define _CRTDBG_MAP_ALLOC
    #define _CRTDBG_MAP_ALLOC_NEW

    #include <stdlib.h>
    #include <crtdbg.h>

    #define new DEBUG_CLIENTBLOCK

#endif



}} // namespace 