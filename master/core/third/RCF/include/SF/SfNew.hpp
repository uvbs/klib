
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_SFNEW_HPP_
#define _SF_SFNEW_HPP_

#include <typeinfo>

#include <SF/Exception.hpp>
#include <SF/Tools.hpp>

namespace SF {

    class Archive;

    template<typename T, typename R>
    R sfNew(T*, R*, Archive &) 
    { 
        return new T; 
    }

    template<typename T, unsigned int N, typename R>
    R sfNew(T (*)[N], R*, Archive &)
    {
        SF_THROW(SF::Exception, "construction not supported")(typeid(T[N])); 
        return NULL;
    }


    // SF_CTOR
    #define SF_CTOR(type, ctor)            inline type *sfNew(type*, type **, SF::Archive &)      { return new ctor; } class RequireSemicolon
    #define SF_CUSTOM_CTOR(type, func)    inline type *sfNew(type*, type **, SF::Archive &)      { type *pt = NULL; func(ar, pt); return pt; } class RequireSemicolon

    // SF_NO_CTOR
    #define SF_NO_CTOR(type)                                                inline type *sfNew(type*, type **, SF::Archive &)            { SF_THROW(SF::Exception, "construction not supported")(typeid(type)); return NULL; } class RequireSemicolon
    #define SF_NO_CTOR_T1(type)            template<typename T>                inline type<T> *sfNew(type<T>*, type<T>**, SF::Archive &)      { SF_THROW(SF::Exception, "construction not supported")(typeid(type<T>)); return NULL; } class RequireSemicolon
    #define SF_NO_CTOR_T2(type)            template<typename T, typename U>    inline type<T,U> *sfNew(type<T,U>*, type<T,U>**, SF::Archive &)    { SF_THROW(SF::Exception, "construction not supported")(typeid(type<T,U>)); return NULL; } class RequireSemicolon

} // namespace SF

#endif // ! _SF_SFNEW_HPP_
