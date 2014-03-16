
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_DEQUE_HPP_
#define _SF_DEQUE_HPP_

#include <deque>

#include <SF/SerializeStl.hpp>

namespace SF {

    // std::deque
    template<typename T, typename A>
    inline void serialize(SF::Archive &ar, std::deque<T,A> &t, const unsigned int version)
    {
        serializeStlContainer<PushBackSemantics>(ar, t, version);
    }

} // namespace SF

#endif // ! _SF_DEQUE_HPP_
