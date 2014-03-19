
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_MAP_HPP_
#define _SF_MAP_HPP_

#include <map>

#include <SF/SerializeStl.hpp>

namespace SF {

    // std::pair
    template<typename T, typename U>
    void serialize(Archive &ar, std::pair<T,U> &t, const unsigned int)
    {
        ar & t.first & t.second;
    }

    // std::map
    template<typename K, typename T1, typename T2, typename A>
    inline void serialize(Archive &ar, std::map<K,T1,T2,A> &t, const unsigned int version)
    {
        serializeStlContainer<InsertSemantics>(ar, t, version);
    }

    // std::multimap
    template<typename K, typename T1, typename T2, typename A>
    inline void serialize(Archive &ar, std::multimap<K,T1,T2,A> &t, const unsigned int version)
    {
        serializeStlContainer<InsertSemantics>(ar, t, version);
    }

}

#endif // ! _SF_MAP_HPP_
