
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_OTEXTSTREAM_HPP_
#define _SF_OTEXTSTREAM_HPP_

#include <SF/Stream.hpp>

namespace SF {

    class OTextStream : public OStream, public WithEncodingText
    {
    public:
        OTextStream(std::ostream &os) : OStream(os) 
        {}
    };

}

#endif // ! _SF_OTEXTSTREAM_HPP_
