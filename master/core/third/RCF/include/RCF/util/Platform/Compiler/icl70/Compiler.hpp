
// Compiler-specific details - Intel C++ 7.0

#ifndef _UTIL_PLATFORM_COMPILER_ICL70_COMPILER_HPP_
#define _UTIL_PLATFORM_COMPILER_ICL70_COMPILER_HPP_

#if !defined(_DEBUG) && !defined(NDEBUG)
#define NDEBUG
#endif

#ifndef __ICC
#define __ICC
#endif

// Obscure linker bug with Intel C++ 8.0, following hack fixes it
#ifdef _MSC_VER
#pragma comment(linker, "/alternatename:??_7?$basic_istream@GU?$char_traits@G@std@@@std@@6B@=__imp_??_7?$basic_istream@GU?$char_traits@G@std@@@std@@6B@")
#pragma comment(linker, "/alternatename:??_7?$basic_ostream@GU?$char_traits@G@std@@@std@@6B@=__imp_??_7?$basic_ostream@GU?$char_traits@G@std@@@std@@6B@")
#pragma comment(linker, "/alternatename:??_8?$basic_istream@GU?$char_traits@G@std@@@std@@7B@=__imp_??_8?$basic_istream@GU?$char_traits@G@std@@@std@@7B@")
#pragma comment(linker, "/alternatename:??_8?$basic_ostream@GU?$char_traits@G@std@@@std@@7B@=__imp_??_8?$basic_ostream@GU?$char_traits@G@std@@@std@@7B@")
#endif

#endif //! _UTIL_PLATFORM_COMPILER_ICL70_COMPILER_HPP_
