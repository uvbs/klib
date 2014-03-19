
//*****************************************************************************
// SF - Serialization Framework
// Copyright (c) 2003. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

#ifndef _UTIL_PLATFORM_HPP_
#define _UTIL_PLATFORM_HPP_

// Compiler
#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#include "Compiler/msvc71/Compiler.hpp"
#elif defined(__INTEL_COMPILER)
#include "Compiler/icl70/Compiler.hpp"
#elif defined(__BORLANDC__)
#include "Compiler/bcc55/Compiler.hpp"
#elif defined(__GNUC__)
#include "Compiler/gcc32/Compiler.hpp"
#elif defined(__MWERKS__)
#include "Compiler/cw80/Compiler.hpp"
#else
#include "Compiler.hpp"
#endif

// Library
#if defined(_MSC_VER) && _MSC_VER < 1310
#include "Library/msvc60/Library.hpp"
#elif defined(_MSC_VER)
#include "Library/msvc71/Library.hpp"
#elif defined(__BORLANDC__)
#include "Library/bcc55/Library.hpp"
#elif defined(__GNUC__)
#include "Library/gcc32/Library.hpp"
#elif defined(__MWERKS__)
#include "Library/cw80/Library.hpp"
#else
#include "Library.hpp"
#endif

// Machine
#if defined(__sparc__) || defined(__sparc)
#include "Machine/SPARC/Machine.hpp"
#elif defined(__x86__) || defined(_M_IX86) || defined(__i386__) || defined(__i386) || defined(i386)
#include "Machine/x86/Machine.hpp"
#else
#include "Machine.hpp"
#endif

// OS
#if defined(__WINDOWS__) || defined(_WIN32)
#include "OS/Windows/OS.hpp"
#elif defined(__CYGWIN__)
#include "OS/Cygwin/OS.hpp"
#elif defined(__unix__)
#include "OS/Unix/OS.hpp"
#else
#include "OS.hpp"
#endif

// System
#if defined(__WINDOWS__) || defined(_WIN32)
#include "System/Windows/System.hpp"
#else
#include "System/NonWindows/System.hpp"
#endif

#endif // ! _UTIL_PLATFORM_HPP_
