
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_STRING_HPP_
#define _SF_STRING_HPP_

#include <string>

#include <SF/Archive.hpp>
#include <SF/SerializeDynamicArray.hpp>

namespace SF {

    // std::basic_string

    template<typename C, typename T, typename A>
    inline void serialize(SF::Archive &ar, std::basic_string<C,T,A> &t, const unsigned int)
    {
        if (ar.isRead()) 
        {
            C *pch = NULL;
            UInt32 length = 0;
            ar & SF::Archive::Flag( SF::Archive::NO_BEGIN_END ) & dynamicArray(pch, length);
            t.assign(pch, length);
            delete [] pch;
        }
        else if (ar.isWrite()) 
        {
            C *pch = const_cast<C *>(t.c_str());
            UInt32 length = static_cast<UInt32>(t.length());
            ar & SF::Archive::Flag( SF::Archive::NO_BEGIN_END ) & dynamicArray(pch, length);
        }
    }

} // namespace SF

#endif // ! _SF_STRING_HPP_
