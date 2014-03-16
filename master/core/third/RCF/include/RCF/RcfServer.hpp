
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _RCF_RCFSERVER_HPP
#define _RCF_RCFSERVER_HPP

#include <deque>
#include <map>
#include <memory>
#include <vector>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <RCF/ClientInfo.hpp>
#include <RCF/Connection.hpp>
#include <RCF/ServerStub.hpp>
#include <RCF/ThreadLibrary.hpp>
#include <RCF/Token.hpp>

namespace RCF {

    class TokenHash 
    {
    public:
        size_t operator()(const Token &token) const;
    };

    class TokenFactory : boost::noncopyable
    {
    public:
        TokenFactory(int tokenCount);
        Token getNextToken();
        void returnToken(Token token);
        std::vector<Token> &getTokenSpace();

    private:
        Token generateToken();

        int nextTokenId_;
        std::vector<Token> tokenSpace_;
        std::vector<Token> tokens_;
        Platform::Threads::mutex mutex_;
    };

    class I_StubFactory
    {
    public:
        virtual ~I_StubFactory() {}
        virtual boost::shared_ptr<I_ServerStub> makeServerStub() = 0;
        virtual std::string getObjectName() = 0;
    };

    template<typename T, typename I_T>
    class StubFactory : public I_StubFactory
    {
    public:
        boost::shared_ptr<I_ServerStub> makeServerStub();
        std::string getObjectName();
    };

    template<typename T, typename I_T>
    class SharedStubFactory : public I_StubFactory
    {
    public:
        boost::shared_ptr<I_ServerStub> makeServerStub();
        boost::shared_ptr<I_ServerStub> makeServerStub(T &x);
        boost::shared_ptr<I_ServerStub> makeServerStub(std::auto_ptr<T> px);
        boost::shared_ptr<I_ServerStub> makeServerStub(boost::shared_ptr<T> px);
        std::string getObjectName();
    };

    template<typename ServerT>
    class ObjectFactory : boost::noncopyable
    {
    public:
        ObjectFactory(ServerT &server);
        Token make(const std::string &objectName);

    private:
        ServerT &server_;
    };
    
    // Stub entry
    class StubEntry : boost::noncopyable
    {
    public:
        StubEntry();
        void reset(boost::shared_ptr<I_ServerStub> stub = boost::shared_ptr<I_ServerStub>(), bool expire = true);
        bool hasStub();
        I_ServerStub &getStub();
        unsigned int &getConnectionCount();
        unsigned int &getTimeStamp();
        bool getExpire() const;
        Platform::Threads::recursive_mutex &getMutex();

    private:
        boost::shared_ptr<I_ServerStub> stub_;
        unsigned int connectionCount_;
        unsigned int timeStamp_;
        bool expire_;
        Platform::Threads::recursive_mutex mutex_;
    };

    // Synchronization - stub entry
    class StubEntryAccess : boost::noncopyable 
    { 
    public: 
        StubEntryAccess(StubEntry &stubEntry);
    private:
        Platform::Threads::recursive_mutex::scoped_lock lock_;
    };

    // Stub entry reference count handler
    class StubEntryAddRef : boost::noncopyable
    {
    public:
        StubEntryAddRef();
        ~StubEntryAddRef();
        void reset(boost::shared_ptr<StubEntry> stubEntry = boost::shared_ptr<StubEntry>());
        
    private:
        void addRef();
        void subRef();
        boost::shared_ptr<StubEntry> stubEntry_;
    };

    struct Session
    {
    public:
        Session(int fd, const ClientInfo &clientInfo, unsigned int recvTimeoutMs) :
            fd(fd),
            clientInfo(clientInfo),
            connection(fd, recvTimeoutMs)
        {}

        int receive();
        int send();

        Connection &getConnection() { return connection; }
        StubEntryAddRef &getStubEntryAddRef() { return stubEntryAddRef; }

    private:
        int fd;
        ClientInfo clientInfo;
        Connection connection;
        StubEntryAddRef stubEntryAddRef;
    };

    class EndpointInfo
    {
    public:
        EndpointInfo();
        EndpointInfo(const std::string &name, const std::string &password, const ClientInfo &clientInfo);
        void addFd(int fd);
        int popFd();
        int frontFd();
        int fdCount();
        std::string getPassword() const;
        bool verifyAccess(const std::string &password, const ClientInfo &clientInfo);
        friend std::ostream &operator<<(std::ostream &os, const EndpointInfo &endpointInfo);

    private:
        std::string name;
        std::string password;
        ClientInfo clientInfo;
        std::deque<int> fds;
    };

    static const int NumberOfWorkerThreads = 1;
    static const int NumberOfTokens = 100;

    template<typename Server> class Multiplexer;
    
    class RcfServer : boost::noncopyable
    {
    private:
        typedef RCF::Session Session;
        friend class SessionManager;
        
    public:
        RcfServer(const std::string &ip, int port, const std::string &endpointName);
        RcfServer(int port);
        ~RcfServer();

        void start(bool wait = false);
        void stop(bool wait = true);
        void reset();

        // preferred form
        //typedef boost::function<void(RcfServer&)> StartCallback;
        // compatible form, needed for Borland C++ 5.5.1
        typedef boost::function1<void, RcfServer&> StartCallback;

        //void setStartCallback(StartCallback startCallback);

        void setStartCallback(StartCallback startCallback) 
        { 
            startCallback_ = startCallback;
        }

        template<typename T> void setStartCallback(void (T::*pfn)(RcfServer &), T &t)
        {
            // this function is implemented here (rather than in RcfServer.inl), because of borland c++ idiosyncracies
            setStartCallback( boost::bind(pfn, &t, _1) );
        }
        void onStart();
        
        template<typename InterfaceT, typename ImplementationT> void bind(bool share = false, std::string name = "");
        template<typename InterfaceT, typename ImplementationT> void bind(ImplementationT &x, std::string name = "");
        template<typename InterfaceT, typename ImplementationT> void bind(std::auto_ptr<ImplementationT> px, std::string name = "");
        template<typename InterfaceT, typename ImplementationT> void bind(boost::shared_ptr<ImplementationT> px, std::string name = "");

        void setAllowedClientIps(const std::vector<std::string> &ips);
        std::vector<std::string> getAllowedClientIps();

        void setNetworkInterface(const std::string &networkInterface);
        std::string getNetworkInterface();

    private:
        template<typename ImplementationT, typename InterfaceT> 
        void bindShared(std::string name, boost::shared_ptr<I_ServerStub> serverStub);

    private:
        friend class Multiplexer<RcfServer>;
        bool handleSession(Session &session);
        Session *createSession(int fd, const ClientInfo &clientInfo);
        void destroySession(Session *session);

    private:

        template<typename ServerT> friend class ObjectFactory;

        void waitForStopEvent();
        
        void sendResponse(Connection &connection, bool oneway);
        void serializeExceptionResponse(Connection &connection);
        
        void createObjectFactory();
        Token createClientStub( const std::string &objectName );
        Token createSharedClientStub( const std::string &objectName );
        bool isShared(const std::string &objectName);
        Token getSharedStubToken(const std::string &objectName);
       
        void incrementStubCount();
        void decrementStubCount();
        void resetStubCount();
        int getStubCount();
        void cleanup( unsigned int timeout = 60 );
        void allocateStubTable();
        void printServerDiagnostics();

    private:

        // Connection parameters
        const int port_;
        int getPort() const { return port_; }

        std::string ip_; // TODO
        std::string getIp() const { return ip_; } 

        static const unsigned int recvTimeoutMs_ = 1000;
        unsigned int getRecvTimeoutMs() const { return recvTimeoutMs_; }

        // Token factory
        TokenFactory tokenFactory_;
        TokenFactory &getTokenFactory();
        
        // Stub table
        typedef boost::shared_ptr<StubEntry> StubEntryPtr;
        typedef Platform::Library::hash_map_t< Token, StubEntryPtr, TokenHash >::Val StubTableT;
        StubTableT &getStubTable();
        StubTableT stubTable_;
        
        int stubCount_;
        Platform::Threads::recursive_mutex stubCountMutex_;
        Platform::Threads::try_mutex cleanupMutex_;
        
        // Multiplexer
        typedef Multiplexer<RcfServer> MultiplexerT;
        std::auto_ptr<MultiplexerT> multiplexer_;
        MultiplexerT &getMultiplexer();

        // Mutex for stop event
        Platform::Threads::mutex stop_mutex_;
        Platform::Threads::mutex &getStopMutex();

        // Condition for stop event
        Platform::Threads::condition stop_event_;
        Platform::Threads::condition &getStopEvent();

        // Flag to stop worker threads
        bool stop_;

        // Flag to indicate if server is running, used to prevent users from starting the server twice
        bool running_;
        
        StubEntry &getStubEntry(Token token);
        boost::shared_ptr<StubEntry> getStubEntryPtr(Token token);

        std::map< std::string, Token > sharedStubTokens;
        std::vector<Token> &getTokenSpace();
    
        // Stub registration and creation
        void registerStubFactory(const std::string &objectName, const std::string &desc, boost::shared_ptr<I_StubFactory> factory);
        I_StubFactory &getStubFactory( const std::string &objectName );

        typedef std::map< std::string, boost::shared_ptr<I_StubFactory> > StubFactoryTableT;
        StubFactoryTableT &getStubFactoryTable();
        StubFactoryTableT stubFactoryTable;

        // start callback
        StartCallback startCallback_;

        //*************************************
        // Endpoints
        std::string endpointName;
        std::string endpointPassword;
        std::string endpointIp;
        int endpointPort;

        void openEndpoint(const std::string &endpointName, std::string &endpointPassword);
        void closeEndpoint(const std::string &endpointName, const std::string &password);
        void reverseThisConnection(const std::string &endpointName, const std::string &password);
        void bindToEndpoint(const std::string &endpointName);
        void spawnReverseConnection(int count, const std::string &ip, int port, const std::string &endpointName, const std::string &endpointPassword);
        void seedRandomNumberGenerator();

        template<typename ServerT> friend class EndpointBroker;
        template<typename ServerT> friend class EndpointServer;
        
        std::map<std::string, EndpointInfo> endpointMap;
        Platform::Threads::mutex endpointMapMutex;
    };

} // namespace RCF

#include <RCF/RcfServer.inl>

#endif // ! _RCF_RCFSERVER_HPP
