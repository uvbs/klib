
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

#ifndef _RCF_MULTIPLEXER_INL_
#define _RCF_MULTIPLEXER_INL_

#include <RCF/Multiplexer.hpp>
#include <RCF/SessionInfo.hpp>
#include <RCF/Tools.hpp>

namespace RCF {

    template<typename MultiplexerT>
    WorkerThread<MultiplexerT>::WorkerThread(MultiplexerT &multiplexer) : 
        multiplexer(multiplexer) 
    {}

    template<typename MultiplexerT>
    void WorkerThread<MultiplexerT>::operator()() 
    {
        // Don't want to leak exceptions through thread boundaries
        try 
        {
            ScopeGuard guard = MakeObjGuard(multiplexer, &MultiplexerT::spawn, 1);
            multiplexer(); 
            guard.Dismiss();
        } 
        catch(...) 
        { 
            RCF_TRACE( "Terminating thread due to uncaught exception" ); 
        }
    }


    // Multiplexer

    template<typename SessionManager>
    Multiplexer<SessionManager>::Multiplexer(SessionManager &sessionManager, int port, int fd) :
        fd_(fd), 
        port_(port),
        stop_(false),
        running_(false),
        networkInterface("0.0.0.0"),
        sessionMaps_(FdPartitionCount),
        //ppCurrentSessionInfo(),
        sessionManager_( sessionManager ),
        allowedIpsMutex_(Platform::Threads::writer_priority)
    {
        for (int i=0; i<FdPartitionCount; i++)
        {
            sessionMaps_[i].first = 
                boost::shared_ptr< Platform::Threads::mutex >( 
                    new Platform::Threads::mutex );
        }
    }

    template<typename SessionManager>
    Multiplexer<SessionManager>::~Multiplexer()
    {
        reset();
    }

    template<typename SessionManager>
    void Multiplexer<SessionManager>::start(int nThreads)
    {
        RCF_TRACE("");

        Platform::Threads::recursive_try_mutex::scoped_lock lock(stopMutex_); RCF_UNUSED_VARIABLE(lock);
        if (!running_) {
            running_ = true;
            stop_ = false;
            init();
            spawn( nThreads );
        }
    }

    template<typename SessionManager>
    void Multiplexer<SessionManager>::stop(bool wait) 
    {
        RCF_TRACE("");

        Platform::Threads::recursive_try_mutex::scoped_lock stopLock(stopMutex_); RCF_UNUSED_VARIABLE(stopLock);
        Platform::Threads::recursive_mutex::scoped_lock workerThreadsLock(workerThreadsMutex_); RCF_UNUSED_VARIABLE(workerThreadsLock);
        stop_ = true;
        if (wait)
        {
            for (unsigned int i=0; i<workerThreads_.size(); i++)
            {
                workerThreads_[i]->join();
            }
            workerThreads_.clear();
            running_ = false;
        }
    }

    template<typename SessionManager>
    void Multiplexer<SessionManager>::reset()
    {
        RCF_TRACE("");

        stop();
        close();
    }

    template<typename SessionManager>
    void Multiplexer<SessionManager>::spawn(int nThreads)
    {
        RCF_TRACE("");

        Platform::Threads::recursive_mutex::scoped_lock spawnLock(spawnMutex_); RCF_UNUSED_VARIABLE(spawnLock);
        Platform::Threads::recursive_try_mutex::scoped_try_lock stopLock(stopMutex_); RCF_UNUSED_VARIABLE(stopLock);
        if (stopLock.locked()) 
        {
            Platform::Threads::recursive_mutex::scoped_lock workerThreadsLock(workerThreadsMutex_); RCF_UNUSED_VARIABLE(workerThreadsLock);
            for (int i=0; i<nThreads; i++)
                workerThreads_.push_back( 
                    boost::shared_ptr<Platform::Threads::thread>(
                        new Platform::Threads::thread( WorkerThread<Multiplexer>(*this) )));
        }
    }

    template<typename SessionManager>
    void Multiplexer<SessionManager>::addFd(int fd)
    {
        RCF_TRACE("")(fd);

        Platform::Threads::recursive_mutex::scoped_lock lock(fdsMutex_); RCF_UNUSED_VARIABLE(lock);
        createSession(fd);
        fds_.insert(fd);
    }

    template<typename SessionManager>
    int Multiplexer<SessionManager>::reserveCurrentSessionForForwarding()
    {
        SessionInfo &sessionInfo = const_cast<SessionInfo &>(getCurrentSessionInfo());
        sessionInfo.setReservedForForwarding(true);
        RCF_TRACE("")(sessionInfo.getFd());
        return sessionInfo.getFd();
    }

    template<typename SessionManager>
    void Multiplexer<SessionManager>::markCurrentSessionForForwarding(int fd)
    {
        SessionInfo &sessionInfo1 = getCurrentSessionInfo();
        SessionInfo &sessionInfo2 = getSession(fd).first;
        RCF_TRACE("")(sessionInfo1.getFd())(sessionInfo2.getFd());
        sessionInfo1.setReservedForForwarding(true);
        //RCF_ASSERT( sessionInfo2.getReservedForForwarding() )(sessionInfo2); // TODO
        sessionInfo1.setForwardFd(sessionInfo2.getFd());
        sessionInfo2.setForwardFd(sessionInfo1.getFd());
    }

    template<typename SessionManager>
    void Multiplexer<SessionManager>::init()
    {
        RCF_TRACE("");

        if (fd_ == -1 && port_ > 0) 
        {
            int ret = 0, err = 0;
            fd_ = static_cast<int>(socket(PF_INET, SOCK_STREAM, IPPROTO_TCP));
            if (fd_ == -1) 
            {
                err = Platform::OS::Socket::GetLastError(); 
                RCF_THROW(MultiplexerException, "failed to create listener socket")(fd_)(err)(Platform::OS::GetErrorString(err)); 
            }
            sockaddr_in serverAddr;
            memset( &serverAddr, 0, sizeof(serverAddr) );
            serverAddr.sin_family = AF_INET;
            if (isdigit(networkInterface.at(0)))
            {
                serverAddr.sin_addr.s_addr = inet_addr( networkInterface.c_str() );
            }
            else
            {
                hostent *h = gethostbyname(networkInterface.c_str());
                if (h) 
                {
                    serverAddr.sin_addr = * (in_addr *) h->h_addr_list[0];
                }
            }
            serverAddr.sin_port = htons(port_);
            ret = bind(fd_, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
            if (ret < 0) 
            {
                err = Platform::OS::Socket::GetLastError(); 
                RCF_THROW(MultiplexerException, "failed to bind listener socket")(fd_)(port_)(networkInterface)(ret)(err)(Platform::OS::GetErrorString(err)); 
            }
            ret = listen(fd_, MaxPendingConnectionCount);
            if (ret < 0) 
            {
                err = Platform::OS::Socket::GetLastError(); 
                RCF_THROW(MultiplexerException, "failed to listen on listener socket")(fd_)(ret)(err)(Platform::OS::GetErrorString(err)); 
            }
            RCF_ASSERT( fd_ != -1 )(fd_);
            insertFd(fd_);
        }
    }

    template<typename SessionManager>
    void Multiplexer<SessionManager>::close()
    {
        RCF_TRACE("");

        if (fd_ != -1)
        {
            int err = Platform::OS::Socket::closesocket(fd_);
            RCF_ASSERT(err == 0)(err)(fd_)(Platform::OS::Socket::GetLastError());
            fd_ = -1;
        }

        {
            Platform::Threads::recursive_mutex::scoped_lock lock( fdsMutex_ ); RCF_UNUSED_VARIABLE(lock);
            fds_.clear();
        }
        {
            Platform::Threads::recursive_mutex::scoped_lock lock( fdsToBeReadMutex_ ); RCF_UNUSED_VARIABLE(lock);
            fdsToBeRead_.clear();
        }

        for (int i=0; i<FdPartitionCount; i++) 
        {
            SessionMap &sessionMap = sessionMaps_[i].second;
            for (typename SessionMap::iterator it = sessionMap.begin(); it != sessionMap.end(); it++)
            {
                clearSession( (*it).first );
            }
            sessionMap.clear();
        }
    }

    template<typename SessionManager>
    void Multiplexer<SessionManager>::operator()()
    {
        RCF_TRACE("");

        try 
        {
            sessionManager_.onStart();
            while (true) 
            {
                int fd = getNextFd();
                ScopeGuard clearSessionGuard = MakeObjGuard(*this, &Multiplexer::clearSession, fd);
                std::pair<SessionInfo, Session*> &s = getSession(fd);
                SessionInfo &sessionInfo = s.first;
                Session &session = *s.second;

                if (sessionInfo.getReservedForForwarding())
                {
                    RCF_TRACE(": handling forwarding session");

                    // TODO: test with small block sizes
                    
                    static const int BlockSize = 1024;
                    std::vector<char> data(BlockSize);
                    int fd = sessionInfo.getFd();
                    int forwardFd = sessionInfo.getForwardFd() != -1 ? sessionInfo.getForwardFd() : fd;
                    RCF_TRACE("")(fd)(forwardFd);
                    int socket_err = 0;
                    int err = 0;

                    err = Platform::OS::Socket::recv(fd, &data[0], BlockSize, 0);
                    if (err < 0)
                    {
                        socket_err = Platform::OS::Socket::GetLastError();
                    }

                    if (err > 0)
                    {
                        int length = err;
                        std::string strData( &data[0], length);
                        RCF_TRACE(": forwarding...")(fd)(forwardFd)(strData.size())(strData.substr(4));
                        err = send(forwardFd, &data[0], length, 0);
                        RCF_ASSERT(err == length)(err)(length);
                        insertFd(fd);
                        clearSessionGuard.Dismiss();
                    }
                    else if (err == 0)
                    {
                        RCF_TRACE("fd closed, posting close request for forwardFd...")(fd)(forwardFd);
                        Platform::Threads::recursive_mutex::scoped_lock lock( fdsToBeClosedMutex_ ); RCF_UNUSED_VARIABLE(lock);
                        fdsToBeClosed_.push_back(forwardFd);
                        clearSession(fd);
                        clearSessionGuard.Dismiss();
                    }
                    else if (err < 0 && socket_err == Platform::OS::Socket::ERR_EWOULDBLOCK)
                    {
                        RCF_TRACE("can't receive on fd just yet (EWOULDBLOCK)")(fd)(forwardFd);
                        insertFd(fd);
                        clearSessionGuard.Dismiss();
                    }
                    else 
                    {
                        RCF_TRACE("error receiving on fd, , posting close request for forwardFd...")(fd)(forwardFd)(socket_err)(Platform::OS::GetErrorString(socket_err));
                        Platform::Threads::recursive_mutex::scoped_lock lock( fdsToBeClosedMutex_ ); RCF_UNUSED_VARIABLE(lock);
                        fdsToBeClosed_.push_back(forwardFd);
                        clearSession(fd);
                        clearSessionGuard.Dismiss();
                    }
                    
                }
                else
                {
                    setCurrentSessionInfo(sessionInfo);
                    ScopeGuard currentSessionInfoGuard = MakeGuard(resetCurrentSessionInfo);
                    RCF_UNUSED_VARIABLE(currentSessionInfoGuard);

                    if (sessionManager_.handleSession( session ))
                    {
                        insertFd(fd);
                        clearSessionGuard.Dismiss();
                    }
                }

            }
        }
        catch (const WorkerThreadTerminator &e) 
        {
            RCF_TRACE("Caught WorkerThreadTerminator exception")(e);
        }
    }

    template<typename SessionManager>
    int Multiplexer<SessionManager>::createSession(int fd)
    {
        RCF_TRACE("")(fd);

        Platform::Threads::mutex &sessionMapMutex = *sessionMaps_[ hash(fd) ].first;
        Platform::Threads::mutex::scoped_lock lock(sessionMapMutex); RCF_UNUSED_VARIABLE(lock);
        SessionMap &sessionMap = sessionMaps_[ hash(fd) ].second;
        Session *session = sessionMap[fd].second;
        RCF_ASSERT( NULL == session )(fd)(session);
        sessionMap[fd] = std::make_pair(SessionInfo(fd), sessionManager_.createSession( fd, ClientInfo() ));
        return fd;
    }

    template<typename SessionManager>
    void Multiplexer<SessionManager>::clearSession(int fd)
    {
        RCF_TRACE("")(fd);

        // TODO: actually remove the element corrresponding to fd, not just blank it?
        Platform::Threads::mutex &sessionMapMutex = *sessionMaps_[ hash(fd) ].first;
        Platform::Threads::mutex::scoped_lock lock(sessionMapMutex); RCF_UNUSED_VARIABLE(lock);
        SessionMap &sessionMap = sessionMaps_[ hash(fd) ].second;
        Session *session = sessionMap[fd].second;
        if (session) 
        {
            int err = Platform::OS::Socket::closesocket(fd);
            RCF_ASSERT(err == 0)(err)(fd)(Platform::OS::Socket::GetLastError());
            sessionMap[fd] = std::make_pair(SessionInfo(), (Session *) NULL);
            sessionManager_.destroySession(session);
        }
    }

    template<typename SessionManager>
    std::pair<SessionInfo, typename Multiplexer<SessionManager>::Session *> &
    Multiplexer<SessionManager>::getSession(int fd)
    {
        RCF_TRACE("")(fd);

        Platform::Threads::mutex &sessionMapMutex = *sessionMaps_[ hash(fd) ].first;
        Platform::Threads::mutex::scoped_lock lock(sessionMapMutex); RCF_UNUSED_VARIABLE(lock);
        SessionMap &sessionMap = sessionMaps_[ hash(fd) ].second;
        return sessionMap[fd];
    }

    template<typename SessionManager>
    void Multiplexer<SessionManager>::insertFd(int fd) 
    {
        RCF_TRACE("")(fd);

        Platform::Threads::recursive_mutex::scoped_lock lock(fdsMutex_); RCF_UNUSED_VARIABLE(lock);
        fds_.insert(fd);
    }

    template<typename SessionManager>
    void Multiplexer<SessionManager>::removeFd(int fd)
    {
        RCF_TRACE("")(fd);

        Platform::Threads::recursive_mutex::scoped_lock lock(fdsMutex_); RCF_UNUSED_VARIABLE(lock);
        fds_.erase(fd);
    }

    template<typename SessionManager>
    int Multiplexer<SessionManager>::getNextFd()
    {
        RCF_TRACE("");

        Platform::Threads::recursive_mutex::scoped_lock lock(fdsToBeReadMutex_); RCF_UNUSED_VARIABLE(lock);

        if ( stop_ ) 
            throw WorkerThreadTerminator();
        
        while ( fdsToBeRead_.empty() ) {

            if ( stop_ ) 
                throw WorkerThreadTerminator();

            // copy fds from set to temp
            std::vector<int> temp;
            {
                Platform::Threads::recursive_mutex::scoped_lock lock(fdsMutex_); RCF_UNUSED_VARIABLE(lock);
                for (std::set<int>::iterator it = fds_.begin(); it != fds_.end(); it++)
                {
                    temp.push_back( *it );
                }
                fds_.clear();
            }

            // see if there are any fds that need to be closed
            {
                Platform::Threads::recursive_mutex::scoped_lock lock(fdsToBeClosedMutex_); RCF_UNUSED_VARIABLE(lock);
                if (!fdsToBeClosed_.empty())
                {
                    std::vector<int> temp2;
                    for (unsigned int i=0; i<temp.size(); i++)
                    {
                        int fd = temp[i];
                        std::vector<int>::iterator iter = std::find(fdsToBeClosed_.begin(), fdsToBeClosed_.end(), fd);
                        if (iter != fdsToBeClosed_.end())
                        {
                            RCF_TRACE("closing and removing socket from select() set")(fd);
                            clearSession(fd);
                            fdsToBeClosed_.erase(iter);
                        }
                        else
                        {
                            temp2.push_back(fd);                                
                        }
                    }
                    temp.swap(temp2);
                }
            }

            // fill the fd_set
            fd_set my_fd_set;
            {
                FD_ZERO( &my_fd_set );
                for (std::vector<int>::iterator it = temp.begin(); it != temp.end(); it++)
                    FD_SET( *it, &my_fd_set );
            }
            
            // call select
            RCF_TRACE("calling select()...")(temp.size())(temp);
            timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            int err = Platform::OS::Socket::select(FD_SETSIZE, &my_fd_set, NULL, NULL, &timeout);
            if (err < 0)
            {
                int socket_err = Platform::OS::Socket::GetLastError();
                RCF_TRACE("select() failed")(err)(socket_err)(Platform::OS::GetErrorString(socket_err));
            }
            else
            {
                Platform::Threads::recursive_mutex::scoped_lock lock(fdsMutex_); RCF_UNUSED_VARIABLE(lock);
                
                if (fd_ != -1)
                {
                    if (FD_ISSET( fd_, &my_fd_set )) 
                    {
                        // accept new connection
                        sockaddr_in addr;
                        memset(&addr, 0, sizeof(sockaddr_in));
                        int addrSize = sizeof(sockaddr_in);
                        int fd = static_cast<int>( Platform::OS::Socket::accept(fd_, (sockaddr *) &addr, &addrSize) );
                        RCF_TRACE( "Accepted new connection" )(fd);
                        Platform::OS::Socket::setblocking(fd, false);
                        if (isClientIpAllowed(addr))
                        {
                            createSession(fd);
                            SessionInfo &sessionInfo = getSession(fd).first;
                            sessionInfo.setClientInfo(ClientInfo(addr));
                            fds_.insert(fd);
                        }
                        else
                        {
                            std::string ip = ::inet_ntoa( addr.sin_addr );
                            RCF_TRACE("Client ip not allowed")(fd)(ip)(allowedIps_);
                            int ret = Platform::OS::Socket::closesocket(fd);
                            RCF_ASSERT(ret == 0)(ret)(fd)(Platform::OS::Socket::GetLastError());
                        }
                    }
                    fds_.insert(fd_);
                }

                // copy readable fds to fdsToBeRead_ and nonreadable to fds_
                for (std::vector<int>::iterator it = temp.begin(); it != temp.end(); it++) {
                    if ( *it != fd_ ) {
                        if ( FD_ISSET( *it, &my_fd_set ) ) 
                            fdsToBeRead_.push_back( *it );
                        else
                            fds_.insert( *it );
                    }
                }

            }
        }

        int fd = fdsToBeRead_.front();
        fdsToBeRead_.pop_front();
        return fd;
    }

    template<typename SessionManager>
    int Multiplexer<SessionManager>::hash(int fd) 
    {
        return fd % FdPartitionCount; // TODO: a better hash function...
    }

    template<typename SessionManager>
    void Multiplexer<SessionManager>::setAllowedClientIps(const std::vector<std::string> &ips)
    {
        AllowedIpsWriteLock lock(allowedIpsMutex_); RCF_UNUSED_VARIABLE(lock);
        allowedIps_ = ips;

        allowedIpAddrs_.clear();
        for (unsigned int i=0; i<allowedIps_.size(); i++)
        {
            const std::string &ip = allowedIps_[i];
            hostent *hostDesc = ::gethostbyname( ip.c_str() );
            if (hostDesc == NULL)
            {
                int err = Platform::OS::Socket::GetLastError();
                std::string strErr = Platform::OS::GetErrorString(err);
                RCF_TRACE("gethostbyname() failed")(ip)(err)(strErr);
                allowedIpAddrs_.push_back(INADDR_NONE);
            }
            else
            {
                in_addr addr = *((in_addr*) hostDesc->h_addr_list[0]);
                allowedIpAddrs_.push_back(addr.s_addr);
            }
        }
    }

    template<typename SessionManager>
    std::vector<std::string> Multiplexer<SessionManager>::getAllowedClientIps()
    {
        AllowedIpsReadLock lock(allowedIpsMutex_); RCF_UNUSED_VARIABLE(lock);
        return allowedIps_;
    }

    template<typename SessionManager>
    bool Multiplexer<SessionManager>::isClientIpAllowed(sockaddr_in &addr)
    {
        AllowedIpsReadLock lock(allowedIpsMutex_); RCF_UNUSED_VARIABLE(lock);

        if (allowedIpAddrs_.empty())
        {
            return true;
        }
        
        std::vector<u_long>::iterator it = 
            std::find(allowedIpAddrs_.begin(), allowedIpAddrs_.end(), addr.sin_addr.s_addr);

        return it != allowedIpAddrs_.end();

    }

} // namespace RCF


#endif //! _RCF_MULTIPLEXER_INL_
