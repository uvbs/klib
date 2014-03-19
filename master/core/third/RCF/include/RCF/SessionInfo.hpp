
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _RCF_SESSIONINFO_HPP_
#define _RCF_SESSIONINFO_HPP_

#include <RCF/ClientInfo.hpp>
#include <RCF/Exception.hpp>
#include <RCF/ThreadLibrary.hpp> // need thread_specific_ptr
#include <RCF/Tools.hpp>

namespace RCF {

    class SessionInfo
    {
    public:
        SessionInfo();
        SessionInfo(int fd);
        void setClientInfo(const ClientInfo &clientInfo) { this->clientInfo = clientInfo; }
        int getFd() const;
        const ClientInfo &getClientInfo() const;
        bool getReservedForForwarding() const;
        int getForwardFd() const;
        void setReservedForForwarding(bool reservedForForwarding);
        void setForwardFd(int forwardFd);
        friend std::ostream &operator<<(std::ostream &os, const SessionInfo &sessionInfo);

    private:
        int fd;
        ClientInfo clientInfo;
        bool reservedForForwarding;
        int forwardFd;
    };

    extern Platform::Threads::thread_specific_ptr<SessionInfo *>::Val ppCurrentSessionInfo;

    // TODO: initialize ppCurrentSessionInfo when each thread starts, instead of the lazy initialization below?

    inline void resetCurrentSessionInfo()
    {
        if (NULL == ppCurrentSessionInfo.get())
        {
            ppCurrentSessionInfo.reset( new SessionInfo*() );
        }
        *ppCurrentSessionInfo = NULL;
    }

    inline void setCurrentSessionInfo(SessionInfo &sessionInfo)
    {
        if (NULL == ppCurrentSessionInfo.get())
        {
            ppCurrentSessionInfo.reset( new SessionInfo*() );
        }
        *ppCurrentSessionInfo = &sessionInfo;
    }

    inline SessionInfo &getCurrentSessionInfo()
    {
        if (NULL == ppCurrentSessionInfo.get())
        {
            ppCurrentSessionInfo.reset( new SessionInfo*() );
        }
        if (*ppCurrentSessionInfo == NULL)
        {
            RCF_THROW(SessionInfoException, "no current session");
        }
        else
        {
            return **ppCurrentSessionInfo;
        }
    }

} // namespace RCF

#endif // ! _RCF_SESSIONINFO_HPP_
