
//*****************************************************************************
// Copyright (c) 2003. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

#ifndef _UTIL_THROW_HPP_
#define _UTIL_THROW_HPP_

#include <exception>

#include <boost/current_function.hpp>

#include "VariableArgMacro.hpp"

namespace util {
    
    template<typename E> 
    struct ThrowFunctor : public VariableArgMacroFunctor
    {
        ~ThrowFunctor() 
        { 
            if (!std::uncaught_exception()) 
            {
                throw E( (header.str() + args.str()).c_str() );
            }
        }
    };


#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4355 )  // warning C4355: 'this' : used in base member initializer list
#endif

#ifdef NDEBUG

    #define UTIL_THROW(E, msg)                throw E(msg); DUMMY_VARIABLE_ARG_MACRO()
    
#else 

    DECLARE_VARIABLE_ARG_MACRO_T1( UTIL_THROW, ThrowFunctor );
    #define UTIL_THROW(E, msg)                while (true) util::VariableArgMacro<util::ThrowFunctor< E > >().init( "THROW : " #E ": ", msg, __FILE__, __LINE__, BOOST_CURRENT_FUNCTION ).cast( (util::VariableArgMacro<util::ThrowFunctor< E > > *) NULL ).UTIL_THROW_A
    #define UTIL_THROW_A(x)                   UTIL_THROW_OP(x, B)
    #define UTIL_THROW_B(x)                   UTIL_THROW_OP(x, A)
    #define UTIL_THROW_OP(x, next)            UTIL_THROW_A.notify_((x), #x).UTIL_THROW_ ## next

#endif

#ifdef _MSC_VER
#pragma warning( pop )
#endif

} // namespace util

#endif // ! _UTIL_THROW_HPP_
