
#ifndef _UTIL_THREADLIBRARY_HPP_
#define _UTIL_THREADLIBRARY_HPP_

#ifdef UTIL_USE_BOOST_THREADS
#include "Platform/Threads/BoostThreads.hpp"
#else
#include "Platform/Threads/ThreadsProxy.hpp"
#endif

#endif // ! _UTIL_THREADLIBRARY_HPP_
