
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_ONATIVEBINARYSTREAM_HPP_
#define _SF_ONATIVEBINARYSTREAM_HPP_

#include <SF/Stream.hpp>

namespace SF {

    class ONativeBinaryStream : public OStream, public WithEncodingBinaryNative
    {
    public:
        ONativeBinaryStream(std::ostream &os) : OStream(os) 
        {}
    };


}

#endif // ! _SF_ONATIVEBINARYSTREAM_HPP_
