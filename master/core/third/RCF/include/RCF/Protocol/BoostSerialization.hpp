
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _RCF_PROTOCOL_BOOSTSERIALIZATION_HPP
#define _RCF_PROTOCOL_BOOSTSERIALIZATION_HPP

#include <RCF/Protocol/Protocol.hpp>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/level.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/utility.hpp>

namespace RCF {

    template<> class Protocol<3> : 
        public ProtocolBase<boost::archive::binary_iarchive, boost::archive::binary_oarchive> 
    {
    public:
        static std::string getName()
        {
            return "boost serialization binary protocol";
        }
    };

    template<> class Protocol<4> : 
        public ProtocolBase<boost::archive::text_iarchive, boost::archive::text_oarchive> 
    {
    public:
        static std::string getName()
        {
            return "boost serialization text protocol";
        }
    };

} // namespace RCF

#endif //! _RCF_PROTOCOL_BOOSTSERIALIZATION_HPP
