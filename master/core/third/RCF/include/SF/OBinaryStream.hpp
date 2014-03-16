
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_OBINARYSTREAM_HPP_
#define _SF_OBINARYSTREAM_HPP_

#include <SF/Stream.hpp>

namespace SF {

    class OBinaryStream : public OStream, public WithEncodingBinaryPortable
    {
    public:
        OBinaryStream(std::ostream &os) : OStream(os) 
        {}
    };

}

#endif // ! _SF_OBINARYSTREAM_HPP_
