
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_TOOLS_HPP_
#define _SF_TOOLS_HPP_

#include <RCF/Tools.hpp>

#define SF_ASSERT(x) RCF_ASSERT(x)
#define SF_TRACE(x) RCF_TRACE(x)
#define SF_THROW(E, msg) RCF_THROW(E, msg)

//****************************************************************************
// Helper macro to generate code for fundamental types
// TODO: add compiler-specific types (__int64 etc) ?

#define FOR_EACH_FUNDAMENTAL_TYPE(arg)    \
    arg(char)                           \
    arg(int)                            \
    arg(bool)                           \
    arg(float)                          \
    arg(double)                         \
    arg(short)                          \
    arg(long)                           \
    arg(unsigned short)                 \
    arg(unsigned char)                  \
    arg(unsigned int)                   \
    arg(unsigned long)                  \
    arg(long double)



#endif // ! _SF_TOOLS_HPP_
