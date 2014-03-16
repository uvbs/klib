
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#include <RCF/RCF.hpp>

namespace RCF {

    RCF_BEGIN( I_ServerInfo, "" )
        RCF_METHOD_R0(std::string, getName)
    RCF_END( I_ServerInfo );

}