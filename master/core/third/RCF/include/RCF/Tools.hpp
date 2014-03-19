
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _RCF_TOOLS_HPP_
#define _RCF_TOOLS_HPP_

// Various utilities

#include <deque>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <typeinfo>
#include <vector>

#include <boost/current_function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include <RCF/Exception.hpp>
#include <RCF/ScopeGuard.h>
#include <RCF/ThreadLibrary.hpp>
#include <RCF/util/Meta.hpp>

#ifdef _MSC_VER
#pragma warning( disable : 4355 )  // warning C4355: 'this' : used in base member initializer list
#endif

// Assertion mechanism
#include <RCF/util/Assert.hpp>
#define RCF_ASSERT(x) UTIL_ASSERT(x, RCF::AssertionFailureException)

// Trace mechanism
#include <RCF/util/Trace.hpp>
#define RCF_TRACE(x) UTIL_TRACE(x, ::RCF::getRcfTraceChannel())
//#define RCF_TRACE(x) DUMMY_VARIABLE_ARG_MACRO()
namespace RCF {
    util::TraceChannel &getRcfTraceChannel();
}

// Throw mechanism
#include <RCF/util/Throw.hpp>
#define RCF_THROW UTIL_THROW

// Eliminate unused variable warnings, eg for scoped lock objects
#define RCF_UNUSED_VARIABLE(x) ((void) x)

#ifndef __BORLANDC__
namespace std {
#endif

    // Trace std::vector
    template<typename T>
    std::ostream &operator<<(std::ostream &os, const std::vector<T> &v)
    {
        os << "(";
        std::copy(v.begin(), v.end(), std::ostream_iterator<T>(os, ", "));
        os << ")";
        return os;
    }

    // Trace std::deque
    template<typename T>
    std::ostream &operator<<(std::ostream &os, const std::deque<T> &d)
    {
        os << "(";
        std::copy(d.begin(), d.end(), std::ostream_iterator<T>(os, ", "));
        os << ")";
        return os;
    }


    // Trace type_info
    inline std::ostream &operator<<(std::ostream &os, const std::type_info &ti) 
    { 
        return os << ti.name(); 
    }
    
    // Trace exception
    inline std::ostream &operator<<(std::ostream &os, const std::exception &e) 
    { 
        return os << "Type: " << typeid(e).name() << ", What: " << e.what(); 
    }

#ifndef __BORLANDC__
} // namespace std
#endif

#endif // ! _RCF_TOOLS_HPP_
