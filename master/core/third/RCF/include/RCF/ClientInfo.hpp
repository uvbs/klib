
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _RCF_CLIENTINFO_HPP_
#define _RCF_CLIENTINFO_HPP_

#include <string>

//struct sockaddr_in;
#include <RCF/util/Platform/Platform.hpp> // need sockets

namespace RCF {

    class ClientInfo 
    {
    public:
        ClientInfo();
        ClientInfo(const sockaddr_in &addr);
        std::string getAddress() const;
        sockaddr_in getSockAddr() const;
    private:
        std::string ip;
        sockaddr_in addr;
    };

} // namespace RCF

#endif // ! _RCF_CLIENTINFO_HPP_
