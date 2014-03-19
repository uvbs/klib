
//*****************************************************************************
// Copyright (c) 2003. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

#ifndef _UTIL_ASSERT_HPP_
#define _UTIL_ASSERT_HPP_

#include <cassert>
#include <exception>

#include <boost/current_function.hpp>

#include "VariableArgMacro.hpp"

//*************************************************
// Assert utility

namespace util {

    template<typename E> 
    struct AssertFunctorDebug : public VariableArgMacroFunctor
    {
        ~AssertFunctorDebug() 
        { 
            assert(0); 
        }
    };

    template<typename E> struct AssertFunctorRelease : public VariableArgMacroFunctor
    {
        ~AssertFunctorRelease() 
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

    DECLARE_VARIABLE_ARG_MACRO_T1( UTIL_ASSERT_DEBUG, AssertFunctorDebug );
    #define UTIL_ASSERT_DEBUG(cond, E)                if (cond) ; else util::VariableArgMacro<util::AssertFunctorDebug< E > >().init( "ASSERT: ", #cond, __FILE__, __LINE__, BOOST_CURRENT_FUNCTION ).cast( (util::VariableArgMacro<util::AssertFunctorDebug< E > > *) NULL).UTIL_ASSERT_DEBUG_A
    #define UTIL_ASSERT_DEBUG_A(x)                    UTIL_ASSERT_DEBUG_OP(x, B)
    #define UTIL_ASSERT_DEBUG_B(x)                    UTIL_ASSERT_DEBUG_OP(x, A)
    #define UTIL_ASSERT_DEBUG_OP(x, next)             UTIL_ASSERT_DEBUG_A.notify_((x), #x).UTIL_ASSERT_DEBUG_ ## next

    DECLARE_VARIABLE_ARG_MACRO_T1( UTIL_ASSERT_RELEASE, AssertFunctorRelease );
    #define UTIL_ASSERT_RELEASE(cond, E)                if (cond) ; else  util::VariableArgMacro<util::AssertFunctorRelease< E > >().init( "ASSERT: ", #cond, __FILE__, __LINE__, BOOST_CURRENT_FUNCTION ).cast( (util::VariableArgMacro<util::AssertFunctorRelease< E > > *) NULL).UTIL_ASSERT_RELEASE_A
    #define UTIL_ASSERT_RELEASE_A(x)                    UTIL_ASSERT_RELEASE_OP(x, B)
    #define UTIL_ASSERT_RELEASE_B(x)                    UTIL_ASSERT_RELEASE_OP(x, A)
    #define UTIL_ASSERT_RELEASE_OP(x, next)             UTIL_ASSERT_RELEASE_A.notify_((x), #x).UTIL_ASSERT_RELEASE_ ## next

    #define UTIL_ASSERT_NULL(cond, E) DUMMY_VARIABLE_ARG_MACRO()

#ifdef _MSC_VER
#pragma warning( pop )
#endif


    #ifdef NDEBUG
    //#define UTIL_ASSERT UTIL_ASSERT_NULL    
    #define UTIL_ASSERT UTIL_ASSERT_RELEASE
    #else
    #define UTIL_ASSERT UTIL_ASSERT_DEBUG
    #endif

} // namespace util

#endif //! _UTIL_ASSERT_HPP_
