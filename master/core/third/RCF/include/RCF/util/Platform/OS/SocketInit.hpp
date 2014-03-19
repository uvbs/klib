
#ifndef _UTIL_PLATFORM_SOCKETINIT_HPP_
#define _UTIL_PLATFORM_SOCKETINIT_HPP_

#if defined(__WINDOWS__) || defined(_WIN32)
#include "Windows/SocketInit.hpp"
#elif defined(__CYGWIN__)
#include "Cygwin/SocketInit.hpp"
#elif defined(__unix__)
#include "Unix/SocketInit.hpp"
#endif

#endif // ! _UTIL_PLATFORM_SOCKETINIT_HPP_
