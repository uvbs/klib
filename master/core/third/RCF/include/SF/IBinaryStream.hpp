
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_IBINARYSTREAM_HPP_
#define _SF_IBINARYSTREAM_HPP_

#include <SF/Stream.hpp>

namespace SF {

    class IBinaryStream : public IStream, public WithEncodingBinaryPortable
    {
    public:
        IBinaryStream(std::istream &is) : IStream(is) 
        {}
    };

}

#endif // ! _SF_IBINARYSTREAM_HPP_
