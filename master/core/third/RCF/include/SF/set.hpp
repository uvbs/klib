
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_SET_HPP_
#define _SF_SET_HPP_

#include <set>

#include <SF/SerializeStl.hpp>

namespace SF {

    // std::set
    template<typename K, typename T, typename A>
    inline void serialize(SF::Archive &ar, std::set<K,T,A> &t, const unsigned int version)
    {
        serializeStlContainer<InsertSemantics>(ar, t, version);
    }

    // std::multiset
    template<typename K, typename T, typename A>
        inline void serialize(SF::Archive &ar, std::multiset<K,T,A> &t, const unsigned int version)
    {
        serializeStlContainer<InsertSemantics>(ar, t, version);
    }

} // namespace SF

#endif // ! _SF_SET_HPP_
