
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _RCF_PROTOCOL_SF_HPP
#define _RCF_PROTOCOL_SF_HPP

#include <RCF/Protocol/Protocol.hpp>

#include <SF/ITextStream.hpp>
#include <SF/OTextStream.hpp>
#include <SF/IBinaryStream.hpp>
#include <SF/OBinaryStream.hpp>
#include <SF/string.hpp>

namespace RCF {

    template<> 
    class Protocol<1> : 
        public ProtocolBase<SF::IBinaryStream, SF::OBinaryStream> 
    {
    public:
        static std::string getName()
        {
            return "SF portable binary protocol";
        }
    };

    template<> 
    class Protocol<2> : 
        public ProtocolBase<SF::ITextStream, SF::OTextStream> 
    {
    public:
        static std::string getName()
        {
            return "SF text protocol";
        }
    };



    // these protocols use placement new to allocate the serialization objects, but
    // it seems to make no performance difference at all...
    template<> 
    class Protocol<5> : 
        public ProtocolBaseWithPlacementNew<SF::IBinaryStream, SF::OBinaryStream> 
    {
    public:
        static std::string getName()
        {
            return "SF portable binary protocol, with placement new";
        }
    };

    template<> 
    class Protocol<6> : 
        public ProtocolBaseWithPlacementNew<SF::ITextStream, SF::OTextStream> 
    {
    public:
        static std::string getName()
        {
            return "SF text protocol, with placement new";
        }
    };

} // namespace RCF



#endif //! _RCF_PROTOCOL_SF_HPP
