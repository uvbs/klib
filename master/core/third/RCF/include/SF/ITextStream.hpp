
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _SF_ITEXTSTREAM_HPP_
#define _SF_ITEXTSTREAM_HPP_

#include <SF/Stream.hpp>

namespace SF {

    class ITextStream : public IStream, public WithEncodingText
    {
    public:
        ITextStream(std::istream &is) : IStream(is) 
        {}
    };

}

#endif // ! _SF_ITEXTSTREAM_HPP_
