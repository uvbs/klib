
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

#include <RCF/Multiplexer.hpp>

namespace RCF {

    // ClientInfo

    ClientInfo::ClientInfo()
    {}

    ClientInfo::ClientInfo(const sockaddr_in &addr) : 
        addr(addr),
        ip(inet_ntoa(addr.sin_addr)) 
    {}

    std::string ClientInfo::getAddress() const 
    { 
        return ip; 
    }

    sockaddr_in ClientInfo::getSockAddr() const
    {
        return addr;
    }

    // SessionInfo

    Platform::Threads::thread_specific_ptr<SessionInfo *>::Val ppCurrentSessionInfo;

    SessionInfo::SessionInfo() : 
        fd(-1), 
        clientInfo(), 
        reservedForForwarding(), 
        forwardFd(-1) 
    {}

    SessionInfo::SessionInfo(int fd) : 
        fd(fd), 
        clientInfo(), 
        reservedForForwarding(), 
        forwardFd(-1) 
    {}

    int SessionInfo::getFd() const 
    { 
        return fd; 
    }
    
    const ClientInfo &SessionInfo::getClientInfo() const 
    { 
        return clientInfo; 
    }
    
    bool SessionInfo::getReservedForForwarding() const 
    { 
        return reservedForForwarding; 
    }
    
    int SessionInfo::getForwardFd() const 
    { 
        return forwardFd; 
    }

    void SessionInfo::setReservedForForwarding(bool reservedForForwarding) 
    { 
        this->reservedForForwarding = reservedForForwarding; 
    }
    
    void SessionInfo::setForwardFd(int forwardFd) 
    { 
        this->forwardFd = forwardFd; 
    }

    std::ostream &operator<<(std::ostream &os, const SessionInfo &sessionInfo)
    {
        os << "(" 
            << " fd = " << sessionInfo.fd 
            << ", reservedForForwarding = " << sessionInfo.reservedForForwarding
            << ", forwardFd = " << sessionInfo.forwardFd
            << ")";

        // TODO
        //os << FORMAT()(fd)(reservedForForwarding)(forwardFd);

        return os;
    }

} // namespace RCF

