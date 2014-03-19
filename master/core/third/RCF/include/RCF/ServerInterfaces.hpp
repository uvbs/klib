
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _RCF_SERVERINTERFACES_HPP
#define _RCF_SERVERINTERFACES_HPP

#ifndef RCF_NO_BOOST_SERIALIZATION
#include <boost/serialization/string.hpp>
#endif

#ifndef RCF_NO_SF_SERIALIZATION
#include <SF/string.hpp>
#endif

#include <RCF/Idl.hpp>

namespace RCF {

    RCF_BEGIN(I_ObjectFactory, "")
        RCF_METHOD_R1(Token, make, const std::string &)
    RCF_END(I_ObjectFactory)

    RCF_BEGIN(I_EndpointBroker, "")
        RCF_METHOD_V2(void, openEndpoint, const std::string &, std::string &)
        RCF_METHOD_V2(void, closeEndpoint, const std::string &, const std::string &)
        RCF_METHOD_V2(void, reverseThisConnection, const std::string &, const std::string &)
        RCF_METHOD_V1(void, bindToEndpoint, const std::string &)
    RCF_END(I_EndpointBroker)

    RCF_BEGIN(I_EndpointServer, "")
        RCF_METHOD_V5(void, spawnReverseConnection, int, const std::string &, int, const std::string &, const std::string &)
    RCF_END(I_EndpointServer)

    RCF_BEGIN( I_ServerInfo, "" )
        RCF_METHOD_R0(std::string, getName)
    RCF_END( I_ServerInfo )

} // namespace RCF

#endif // ! _RCF_SERVERINTERFACES_HPP
