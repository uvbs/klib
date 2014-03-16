
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_SERIALIZEENUM_HPP_
#define _SF_SERIALIZEENUM_HPP_

#define SF_SERIALIZE_ENUM(type)                                     \
    template<typename Archive>                                      \
    void serialize(Archive &ar, type &t, const unsigned int)        \
    {                                                               \
        if (ar.isRead())                                            \
        {                                                           \
            int n = 0;                                              \
            ar & n;                                                 \
            t = type(n);                                            \
        }                                                           \
        else /* if (ar.isWrite())) */                               \
        {                                                           \
            int n = t;                                              \
            ar & n;                                                 \
        }                                                           \
    }

#endif // ! _SF_SERIALIZEENUM_HPP_
