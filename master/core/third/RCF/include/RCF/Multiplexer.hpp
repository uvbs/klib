
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _RCF_MULTIPLEXER_HPP_
#define _RCF_MULTIPLEXER_HPP_

#include <deque>
#include <map>
#include <set>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <RCF/ClientInfo.hpp>
#include <RCF/ScopeGuard.h>
#include <RCF/SessionInfo.hpp>
#include <RCF/ThreadLibrary.hpp>
#include <RCF/util/Platform/Platform.hpp> // need sockets

namespace RCF {

    template<typename MultiplexerT>
    class WorkerThread 
    {
    public:
        WorkerThread(MultiplexerT &multiplexer);
        void operator()();
    private:
        MultiplexerT &multiplexer;
    };

    class WorkerThreadTerminator : public std::exception 
    {};

    static const int MaxPendingConnectionCount = 100;
    static const int FdPartitionCount = 16;

    template<typename SessionManager>
    class Multiplexer : boost::noncopyable 
    {
    public:
        typedef typename SessionManager::Session Session;
                
        Multiplexer(SessionManager &sessionManager, int port = 0, int fd = -1);
        ~Multiplexer();
        
        void start(int nThreads);
        void stop(bool wait = true);
        void reset();
        void spawn(int nThreads);
        void operator()();

        void addFd(int fd);
        int reserveCurrentSessionForForwarding();
        void markCurrentSessionForForwarding(int fd);
        //SessionInfo &getCurrentSessionInfo();

        void setAllowedClientIps(const std::vector<std::string> &ips);
        std::vector<std::string> getAllowedClientIps();

        void setNetworkInterface(const std::string &networkInterface)
        {
            this->networkInterface = networkInterface;
        }
        std::string getNetworkInterface()
        {
            return networkInterface;
        }

    private:
        bool isClientIpAllowed(sockaddr_in &addr);
        
    private:
        //void resetCurrentSessionInfo();
        //void setCurrentSessionInfo(SessionInfo &sessionInfo);
        void init();
        void close();
        
        std::pair<SessionInfo, Session *> &getSession(int fd);
        int createSession(int fd);
        void clearSession(int fd);
        void insertFd(int fd);
        void removeFd(int fd);
        int getNextFd();
        int hash(int fd);

        Platform::Threads::recursive_mutex fdsMutex_;
        Platform::Threads::recursive_mutex fdsToBeReadMutex_;
        Platform::Threads::recursive_mutex fdsToBeClosedMutex_;
        Platform::Threads::recursive_mutex workerThreadsMutex_;
        Platform::Threads::recursive_try_mutex stopMutex_;
        Platform::Threads::recursive_mutex spawnMutex_;
        std::vector< boost::shared_ptr<Platform::Threads::thread> > workerThreads_;

        std::vector<std::string> allowedIps_;
        std::vector<u_long> allowedIpAddrs_;
        Platform::Threads::read_write_mutex allowedIpsMutex_;
        typedef Platform::Threads::read_write_mutex::scoped_read_lock AllowedIpsReadLock;
        typedef Platform::Threads::read_write_mutex::scoped_write_lock AllowedIpsWriteLock;
                
        int fd_;
        int port_;
        bool stop_;
        bool running_;
        std::set<int> fds_;
        std::deque<int> fdsToBeRead_;
        std::vector<int> fdsToBeClosed_;
        std::string networkInterface;
        
        typedef std::map<int, std::pair<SessionInfo, Session *> > SessionMap;
        typedef std::pair<boost::shared_ptr<Platform::Threads::mutex>, SessionMap > SessionMapEntry;
        typedef std::vector<SessionMapEntry> SessionMaps;
        SessionMaps sessionMaps_;

        //boost::thread_specific_ptr<SessionInfo *> ppCurrentSessionInfo;
        //Platform::Threads::thread_specific_ptr<SessionInfo *>::Val ppCurrentSessionInfo;

        SessionManager &sessionManager_;
    };

} // namespace RCF

#include <RCF/Multiplexer.inl>

#endif //! _RCF_MULTIPLEXER_HPP_
