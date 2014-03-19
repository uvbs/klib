// Compiler-specific details - Metrowerks CodeWarrior 8.0

#ifndef _UTIL_PLATFORM_COMPILER_CW80_COMPILER_HPP_
#define _UTIL_PLATFORM_COMPILER_CW80_COMPILER_HPP_

// Linker problem: which lib file is __CrtDbgReport defined in?
// For now, we'll just redirect the ATLASSERT macro...
#define _ATL_NO_DEBUG_CRT
#define ATLASSERT(arg) assert(arg)

#endif //! _UTIL_PLATFORM_COMPILER_CW80_COMPILER_HPP_
