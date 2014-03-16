
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

#include <RCF/RcfServer.hpp>

#include <time.h>

#include <boost/lexical_cast.hpp>
#include <RCF/Idl.hpp>
#include <RCF/MethodInvocation.hpp>
#include <RCF/Multiplexer.hpp>
#include <RCF/Random.hpp>
#include <RCF/ServerInterfaces.hpp>
#include <RCF/Token.hpp>
#include <RCF/Tools.hpp>

namespace RCF {

    // time in s since ca 1970, may fail after year 2038
    unsigned int getCurrentTimeS() 
    { 
        return static_cast<unsigned int>(time(NULL)); 
    }

    
    //*****************************
    // TokenFactory

    size_t TokenHash::operator()(const Token &token) const 
    {
        return token.getId();
    } 

    TokenFactory::TokenFactory(int tokenCount) 
        : nextTokenId_(1) 
    {
        for (int i=0; i<tokenCount; i++) 
            tokenSpace_.push_back( generateToken() );
            
        tokens_.assign( tokenSpace_.rbegin(), tokenSpace_.rend() );
        
        // VC6 library deficiencies - above code is equivalent to following:
        //for (int i=static_cast<int>(tokenSpace_.size())-1; i>=0; i--)
        //    tokens_.push_back( tokenSpace_[i] );
        
    }
    
    Token TokenFactory::getNextToken() 
    {
        Platform::Threads::mutex::scoped_lock lock(mutex_); (void) RCF_UNUSED_VARIABLE(lock);

        if (tokens_.empty())
            RCF_THROW( RCF::ServerException, "no more tokens available" );

        Token token = tokens_.back();
        tokens_.pop_back();
        return token;
    }
    
    void TokenFactory::returnToken(Token token) 
    { 
        Platform::Threads::mutex::scoped_lock lock(mutex_); RCF_UNUSED_VARIABLE(lock);
        tokens_.push_back(token);
    }

    std::vector<Token> &TokenFactory::getTokenSpace() 
    { 
        return tokenSpace_; 
    }

    Token TokenFactory::generateToken() 
    {
        return Token(nextTokenId_++);
    }

    //********************************************
    // StubEntry

    StubEntry::StubEntry() : 
        stub_(), 
        connectionCount_(), 
        timeStamp_(), 
        expire_(),
        mutex_() 
    {}

    void StubEntry::reset(boost::shared_ptr<I_ServerStub> stub, bool expire)
    {
        stub_ = stub;
        connectionCount_ = 0;
        timeStamp_ = getCurrentTimeS();
        expire_ = expire;
    }

    bool StubEntry::hasStub()
    {
        return stub_.get() != NULL;
    }

    I_ServerStub &StubEntry::getStub() 
    { 
        return *stub_; 
    }
    
    unsigned int &StubEntry::getConnectionCount() 
    { 
        return connectionCount_; 
    }
    
    unsigned int &StubEntry::getTimeStamp() 
    { 
        return timeStamp_; 
    }

    bool StubEntry::getExpire() const
    {
        return expire_;
    }
    
    Platform::Threads::recursive_mutex &StubEntry::getMutex() 
    { 
        return mutex_; 
    }

    // StubEntryAccess
    StubEntryAccess::StubEntryAccess(StubEntry &stubEntry) 
        : lock_( stubEntry.getMutex() ) 
    {}

    // StubEntryAddRef
    StubEntryAddRef::StubEntryAddRef() :
      stubEntry_()
    {}
    
    StubEntryAddRef::~StubEntryAddRef() 
    { 
        reset();
    }

    void StubEntryAddRef::reset(boost::shared_ptr<StubEntry> stubEntry) 
    { 
        subRef(); 
        stubEntry_ = stubEntry; 
        addRef(); 
    }

    void StubEntryAddRef::addRef() 
    { 
        if (stubEntry_)
        {
            StubEntryAccess access(*stubEntry_);
            stubEntry_->getConnectionCount()++;
        }
    }
    
    void StubEntryAddRef::subRef() 
    { 
        if (stubEntry_)
        {
            StubEntryAccess access(*stubEntry_);
            stubEntry_->getConnectionCount()--;
            stubEntry_->getTimeStamp() = getCurrentTimeS(); 
        }
    }
    
    int Session::receive() 
    { 
        return connection.receive(); 
    }
    
    int Session::send() 
    { 
        return connection.send(); 
    }


    //*******************************
    // EndpointInfo

    EndpointInfo::EndpointInfo() 
    {}

    EndpointInfo::EndpointInfo(const std::string &name, const std::string &password, const ClientInfo &clientInfo) : 
        name(name), 
        password(password),
        clientInfo(clientInfo) 
    {}
    
    void EndpointInfo::addFd(int fd) 
    { 
        RCF_TRACE("")(fd);
        fds.push_back(fd); 
    }

    int EndpointInfo::popFd() 
    {
        RCF_TRACE("")(fds.front());
        int fd = fds.front(); 
        fds.pop_front(); 
        return fd; 
    }

    int EndpointInfo::frontFd() 
    { 
        return fds.front(); 
    }
    
    int EndpointInfo::fdCount() 
    { 
        return static_cast<int>(fds.size()); 
    }

    std::string EndpointInfo::getPassword() const 
    { 
        return password; 
    }

    bool EndpointInfo::verifyAccess(const std::string &password, const ClientInfo &clientInfo)
    {
        return 
            clientInfo.getAddress() == this->clientInfo.getAddress() &&
            password == this->password;
    }

    std::ostream &operator<<(std::ostream &os, const EndpointInfo &endpointInfo)
    {
        os << "(" 
            << "name = " << endpointInfo.name << ", "
            << "password = " << endpointInfo.password << ", "
            << "fds = " << endpointInfo.fds
            << ")";
        return os;
    }
   

    //*******************************
    // RcfServer

    template<typename Server>
    class EndpointBroker
    {
    public:
        EndpointBroker(Server &server) : server(server) {}
        void openEndpoint(const std::string &name, std::string &password) { server.openEndpoint(name, password); }
        void closeEndpoint(const std::string &name, const std::string &password) { server.closeEndpoint(name, password); }
        void reverseThisConnection(const std::string &name, const std::string &password) { server.reverseThisConnection(name, password); }
        void bindToEndpoint(const std::string &name) { server.bindToEndpoint(name); }
    private:
        Server &server;
    };

    template<typename Server>
    class EndpointServer
    {
    public:
        EndpointServer(Server &server) : server(server) {}
        void spawnReverseConnection(
            int count, 
            const std::string &ip, 
            int port, 
            const std::string &endpointName, 
            const std::string &endpointPassword) 
        { 
            server.spawnReverseConnection(count, ip, port, endpointName, endpointPassword); 
        }

    private:
        Server &server;
    };

    class ServerInfo
    {
    public:
        ServerInfo(const std::string &name) : name(name) {}
        std::string getName() { return name; }
    private:
        std::string name;
    };

    RcfServer::RcfServer(int port) : 
        port_(port), 
        tokenFactory_(NumberOfTokens),
        multiplexer_( new MultiplexerT(*this, port_) ),
        running_(false),
        startCallback_()
    {
        RCF_TRACE("")(port);

        allocateStubTable();
        createObjectFactory();
    }

    RcfServer::RcfServer(const std::string &endpointIp, int endpointPort, const std::string &endpointName) : 
        port_(), 
        tokenFactory_(NumberOfTokens),
        multiplexer_( new MultiplexerT(*this) ),
        running_(false),
        startCallback_(),
        endpointName(endpointName),
        endpointIp(endpointIp),
        endpointPort(endpointPort)
    {
        RCF_TRACE("")(endpointIp)(endpointPort)(endpointName);

        allocateStubTable();
        createObjectFactory();
    }
    
    RcfServer::~RcfServer() 
    {
        RCF_TRACE("");

        reset();
    }

    void RcfServer::stop(bool wait)
    {
        RCF_TRACE("");

        stop_ = true;
        getMultiplexer().stop(wait);
        getStopEvent().notify_all();
        stop_ = false;
        running_ = false;
    }

    void RcfServer::reset()
    {
        RCF_TRACE("");

        stop();
        getMultiplexer().reset();
        cleanup(0);
    }

    void RcfServer::onStart()
    {
        if (startCallback_)
        {
            startCallback_(*this);
        }
    }

    void RcfServer::waitForStopEvent()
    {
        RCF_TRACE("");

        Platform::Threads::mutex::scoped_lock lock( getStopMutex() ); RCF_UNUSED_VARIABLE(lock);
        getStopEvent().wait( lock );
    }

    void RcfServer::setAllowedClientIps(const std::vector<std::string> &ips)
    {
        getMultiplexer().setAllowedClientIps(ips);
    }
    
    std::vector<std::string> RcfServer::getAllowedClientIps()
    {
        return getMultiplexer().getAllowedClientIps();
    }

    void RcfServer::setNetworkInterface(const std::string &networkInterface)
    {
        getMultiplexer().setNetworkInterface(networkInterface);
    }
    std::string RcfServer::getNetworkInterface()
    {
        return getMultiplexer().getNetworkInterface();
    }

    void RcfServer::serializeExceptionResponse(Connection &connection)
    {
        RCF_TRACE("");

        connection.prepareSend();
        connection << RCF::MethodInvocationResponse(true);
        connection << std::string("std::runtime_error");
        connection << std::string("Non-std::exception derived exception was thrown upon invoking server object");
    }

    void RcfServer::sendResponse(Connection &connection, bool oneway)
    {
        RCF_TRACE("");

        if (!oneway)
        {
            connection.send();
        }
    }

    Session *RcfServer::createSession(int fd, const ClientInfo &clientInfo) 
    { 
        return new Session(fd, clientInfo, getRecvTimeoutMs());
    }

    void RcfServer::destroySession(Session *session) 
    { 
        delete session;
    }

    bool RcfServer::handleSession(Session &session) 
    {
        RCF_TRACE("");

        StubEntryAddRef &stubEntryAddRef = session.getStubEntryAddRef();
        Connection &connection = session.getConnection();
        MethodInvocationRequest request;

        if (connection.receive() <= 0)
        {
            return false;
        }
        
        connection >> request;
        
        if (request.getClose()) 
        {
            return false;
        }

        bool oneway = request.getOneway();
                        
        Token token = request.getToken();
        if (token == Token())
        {
            std::string service = request.getService();
            token = getSharedStubToken(service);
        }
        StubEntry &stubEntry = getStubEntry(token);
        stubEntryAddRef.reset( getStubEntryPtr(token) );
        
        {
            // TODO: the following two scopes are apparently not triggered by Borland C++, when throwing non
            // std::exception derived exceptions.
            ScopeGuard sendResponseGuard = 
                MakeObjGuard(*this, &RcfServer::sendResponse, ByRef(connection), oneway);

            {

                ScopeGuard serializeExceptionResponseGuard = 
                    MakeObjGuard(*this, &RcfServer::serializeExceptionResponse, ByRef(connection)) ;

                try 
                {
                    stubEntry.getStub().invoke(request.getFnId(), connection);
                    serializeExceptionResponseGuard.Dismiss();
                }
                catch(const SerializationException &e) 
                {
                    RCF_TRACE(": Serialization exception")(typeid(e))(e);
                    serializeExceptionResponseGuard.Dismiss();
                    sendResponseGuard.Dismiss();
                    return false;
                }
                catch(const MultiplexerException &e) 
                {
                    RCF_TRACE(": Multiplexer exception")(typeid(e))(e);
                    serializeExceptionResponseGuard.Dismiss();
                    sendResponseGuard.Dismiss();
                    return false;
                }
                catch(const std::exception &e) 
                {
                    RCF_TRACE(": User exception")(typeid(e))(e);
                    serializeExceptionResponseGuard.Dismiss();
                    connection.prepareSend();
                    connection << RCF::MethodInvocationResponse(true);
                    connection << std::string(typeid(e).name()); // TODO: something more predictable typeid.name()
                    connection << std::string(e.what());
                }
            }
        }

        return true;
    }

    void RcfServer::createObjectFactory()
    {
        RCF_TRACE("");

        bind< I_ObjectFactory,  ObjectFactory<RcfServer> >(
            std::auto_ptr< ObjectFactory<RcfServer> >( new ObjectFactory<RcfServer>(*this) ));

        bind< I_EndpointBroker, EndpointBroker<RcfServer> >(
            std::auto_ptr< EndpointBroker<RcfServer> >( new EndpointBroker<RcfServer>(*this) ));

        bind< I_EndpointServer, EndpointServer<RcfServer> >(
            std::auto_ptr< EndpointServer<RcfServer> >( new EndpointServer<RcfServer>(*this) ));

        bind< I_ServerInfo, ServerInfo >(
            std::auto_ptr<ServerInfo>( new ServerInfo(endpointName) ));
    }

    Token RcfServer::createClientStub( const std::string &objectName ) 
    {
        RCF_TRACE("")(objectName);

        if (isShared(objectName))
        {
            return createSharedClientStub(objectName);
        }
        else
        {
            I_StubFactory &factory = getStubFactory( objectName );
            boost::shared_ptr<I_ServerStub> serverStub = factory.makeServerStub();
            Token token = getTokenFactory().getNextToken();
            serverStub->setToken( token );
            RCF_ASSERT( getStubTable().find( token ) != getStubTable().end() );
            StubEntry &stubEntry = getStubEntry(token);
            StubEntryAccess access(stubEntry);
            RCF_ASSERT( ! stubEntry.hasStub() );
            stubEntry.reset(serverStub);
            incrementStubCount();
            return token;
        }
    }

    Token RcfServer::createSharedClientStub( const std::string &objectName )
    {
        RCF_TRACE("")(objectName);

        return getSharedStubToken(objectName);
    }

    bool RcfServer::isShared(const std::string &objectName)
    {
        RCF_TRACE("")(objectName);

        return sharedStubTokens.find(objectName) != sharedStubTokens.end();
    }

    Token RcfServer::getSharedStubToken(const std::string &objectName)
    {
        RCF_TRACE("")(objectName);

        RCF_ASSERT(isShared(objectName))(objectName);
        return sharedStubTokens[ objectName ];
    }

    std::vector<Token> &RcfServer::getTokenSpace() 
    { 
        return getTokenFactory().getTokenSpace(); 
    }

    void RcfServer::incrementStubCount()
    {
        Platform::Threads::recursive_mutex::scoped_lock lock( stubCountMutex_ ); RCF_UNUSED_VARIABLE(lock);
        stubCount_++;
    }

    void RcfServer::decrementStubCount()
    {
        Platform::Threads::recursive_mutex::scoped_lock lock( stubCountMutex_ ); RCF_UNUSED_VARIABLE(lock);
        stubCount_--;
    }

    void RcfServer::resetStubCount()
    {
        Platform::Threads::recursive_mutex::scoped_lock lock( stubCountMutex_ ); RCF_UNUSED_VARIABLE(lock);
        stubCount_ = 0;
    }

    int RcfServer::getStubCount()
    {
        Platform::Threads::recursive_mutex::scoped_lock lock( stubCountMutex_ ); RCF_UNUSED_VARIABLE(lock);
        return stubCount_;
    }

    void RcfServer::cleanup(unsigned int timeout) 
    {

        Platform::Threads::try_mutex::scoped_try_lock lock( cleanupMutex_ ); RCF_UNUSED_VARIABLE(lock);
        if (!lock.locked())
            return;

        // Clean up the stub table
        RCF_TRACE("");
        for (std::vector<Token>::iterator iter = getTokenSpace().begin(); iter != getTokenSpace().end(); iter++) {
            Token token = *iter;
            StubEntry &stubEntry = getStubEntry(token);
            if (stubEntry.getExpire()) {
                if (stubEntry.hasStub()) {
                    if (stubEntry.getConnectionCount() == 0) {
                        bool bReturnToken = false;
                        {
                            // Server stubs are removed when a minimum of timeout seconds have passed since their connection count went to zero
                            StubEntryAccess access(stubEntry);
                            if (stubEntry.getExpire() &&
                                stubEntry.getConnectionCount() == 0 && 
                                stubEntry.getTimeStamp() != 0 && 
                                (getCurrentTimeS() - stubEntry.getTimeStamp()) > timeout ) { 
                                     stubEntry.reset();
                                    bReturnToken = true;
                                    decrementStubCount();
                                    RCF_TRACE("Stub entry cleared")(token);
                            }
                        }
                        if (bReturnToken) 
                        {
                            getTokenFactory().returnToken(token);
                            RCF_TRACE("Token returned")(token);
                        }
                    }
                }
            }
        }
    }    

    void RcfServer::allocateStubTable()
    {
        RCF_TRACE("");

        if (! running_) {
            getStubTable().clear();
            for (std::vector<Token>::iterator iter = getTokenSpace().begin(); iter != getTokenSpace().end(); iter++) 
                getStubTable()[*iter] = boost::shared_ptr<StubEntry>( new StubEntry() );
            resetStubCount();
        }
        else
            RCF_TRACE( "Can't allocate stub table while server is running!" );
    }

    void RcfServer::printServerDiagnostics() 
    {
        RCF_TRACE("");

        int used = 0;
        int unused = 0;

        RCF_TRACE("Stub table contents");
        for (std::vector<Token>::iterator iter = getTokenSpace().begin(); iter != getTokenSpace().end(); iter++) {
            Token token = *iter;
            StubEntry &stubEntry = getStubEntry(token);
            if (stubEntry.hasStub()) {
                RCF_TRACE("Stub entry: ")(token)(stubEntry.getConnectionCount())(stubEntry.getTimeStamp());
                used++;
            }
            else
                unused++;
        }
        RCF_TRACE("Stub count: ")(used)(unused);
    }

    TokenFactory &RcfServer::getTokenFactory() 
    { 
        return tokenFactory_; 
    }
    
    RcfServer::StubTableT &RcfServer::getStubTable() 
    { 
        return stubTable_; 
    }
    
    // Multiplexer
    RcfServer::MultiplexerT &RcfServer::getMultiplexer() 
    { 
        return *multiplexer_; 
    }

    Platform::Threads::mutex &RcfServer::getStopMutex() 
    { 
        return stop_mutex_; 
    }

    Platform::Threads::condition &RcfServer::getStopEvent() 
    { 
        return stop_event_; 
    }

    StubEntry &RcfServer::getStubEntry(Token token)
    {
        return *getStubEntryPtr(token);
    }

    boost::shared_ptr<StubEntry> RcfServer::getStubEntryPtr(Token token)
    {
        if (getStubTable().find(token) != getStubTable().end())
            return getStubTable()[token];
        else
            RCF_THROW( RCF::ServerException, "token not found in stub table" )(token);
    }

    void RcfServer::registerStubFactory(const std::string &objectName, const std::string &/*desc*/, boost::shared_ptr<I_StubFactory> factory)
    {
        RCF_TRACE("")(objectName)((int)getStubFactoryTable().size());
        getStubFactoryTable()[ objectName ] = factory;
    }

    I_StubFactory &RcfServer::getStubFactory( const std::string &objectName )
    {
        if (getStubFactoryTable().find(objectName) == getStubFactoryTable().end())
            RCF_THROW(RCF::ServerException, "stub factory not registered")(objectName);
        return * getStubFactoryTable()[ objectName ];
    }

    RcfServer::StubFactoryTableT &RcfServer::getStubFactoryTable() 
    {
        return stubFactoryTable;
    }

    

    void RcfServer::start(bool wait)
    {
        RCF_TRACE("")(wait);

        if ( ! running_ ) {
            running_ = true;
            stop_ = false;
            //getMultiplexer().reset();
            
            if (endpointName != "")
            {
                // Passive server code for endpoints
                RcfClient<I_EndpointBroker> endpointBroker(endpointIp, endpointPort);
                endpointBroker.openEndpoint(endpointName, endpointPassword);
                getMultiplexer().addFd( endpointBroker.releaseConnection()->releaseFd() );
                getMultiplexer().start( NumberOfWorkerThreads );
            }
            else
            {
                getMultiplexer().start( NumberOfWorkerThreads );
                if (wait)
                {
                    waitForStopEvent();
                }
            }
        }
    }
    

    void RcfServer::openEndpoint(const std::string &endpointName, std::string &endpointPassword)
    {
        RCF_TRACE("")(endpointName)(endpointPassword);
        
        Platform::Threads::mutex::scoped_lock lock(endpointMapMutex); RCF_UNUSED_VARIABLE(lock);
        if (endpointMap.find(endpointName) == endpointMap.end())
        {
            {
                // TODO: better passwords
                int n = generateRandomNumber();
                endpointPassword = boost::lexical_cast<std::string>(n);
            }
            
            EndpointInfo endpointInfo(
                endpointName, 
                endpointPassword,
                getCurrentSessionInfo().getClientInfo());

            endpointMap[endpointName] = endpointInfo;

            int fd = getMultiplexer().reserveCurrentSessionForForwarding();
            endpointMap[endpointName].addFd(fd);
        }
    }

    void RcfServer::closeEndpoint(const std::string &endpointName, const std::string &endpointPassword)
    {
        RCF_TRACE("")(endpointName)(endpointPassword);

        Platform::Threads::mutex::scoped_lock lock(endpointMapMutex); RCF_UNUSED_VARIABLE(lock);
        if (endpointMap.find(endpointName) != endpointMap.end())
        {
            EndpointInfo &endpointInfo = endpointMap[endpointName];
            const ClientInfo &clientInfo = getCurrentSessionInfo().getClientInfo();
            if (endpointInfo.verifyAccess(endpointPassword, clientInfo))
            {
                endpointMap.erase( endpointMap.find(endpointName) );
            }
        }
    }

    void RcfServer::reverseThisConnection(const std::string &endpointName, const std::string &endpointPassword)
    {
        RCF_TRACE("")(endpointName)(endpointPassword);

        Platform::Threads::mutex::scoped_lock lock(endpointMapMutex); RCF_UNUSED_VARIABLE(lock);
        if (endpointMap.find(endpointName) != endpointMap.end())
        {
            EndpointInfo &endpointInfo = endpointMap[endpointName];
            const ClientInfo &clientInfo = getCurrentSessionInfo().getClientInfo();
            if (endpointInfo.verifyAccess(endpointPassword, clientInfo))
            {
                int fd = getMultiplexer().reserveCurrentSessionForForwarding();
                endpointInfo.addFd(fd);
            }
            else
            {
                throw RCF::Exception("Invalid password");
            }
        }
    }

    bool isFdWriteable(int fd)
    {
        fd_set fdSet;
        FD_ZERO(&fdSet);
        FD_SET(fd, &fdSet);
        int err = select(FD_SETSIZE, NULL, &fdSet, NULL, NULL);
        return err == 1;
    }

    bool sendOnewayRemoteSpawnRequest(int fd, int count, const std::string &ip, int port, const std::string &endpointName, const std::string &endpointPassword)
    {
        RCF_TRACE("")(fd)(count)(ip)(port)(endpointName)(endpointPassword);
        try
        {
            RcfClient<I_EndpointServer> endpointServer;
            endpointServer.setFd(fd);
            endpointServer.setOneway(true);
            endpointServer.spawnReverseConnection(count, ip, port, endpointName, endpointPassword);
            return true;
        }
        catch(const RCF::Exception &e)
        {
            RCF_TRACE("failed!")(e);
            return false;
        }
    }

    void RcfServer::bindToEndpoint(const std::string &endpointName)
    {
        RCF_TRACE("")(endpointName);

        Platform::Threads::mutex::scoped_lock lock(endpointMapMutex); RCF_UNUSED_VARIABLE(lock);
        if (endpointMap.find(endpointName) != endpointMap.end())
        {
            EndpointInfo &endpointInfo = endpointMap[endpointName];
            RCF_TRACE("")(endpointInfo)(endpointInfo.fdCount());
            while (endpointInfo.fdCount() > 1)
            {
                int fd = endpointInfo.popFd();
                if (isFdWriteable(fd))
                {
                    getMultiplexer().markCurrentSessionForForwarding(fd);
                    return;
                }
            }
            if (endpointInfo.fdCount() == 1 )
            {
                int fd = endpointInfo.frontFd();
                std::string endpointPassword = endpointInfo.getPassword();
                if (sendOnewayRemoteSpawnRequest(fd, 3, "localhost", port_, endpointName, endpointPassword))
                {
                    RCF_THROW(RCF::ServerException, "no available fds, issuing remote spawn request...");
                }
                else
                {
                    endpointMap.erase( endpointMap.find( endpointName ) );
                    RCF_THROW(RCF::ServerException, "no contact with endpoint, closing endpoint...");
                }
            }
            else
            {
                RCF_THROW(RCF::ServerException, "no available fds, cant even issue a remote spawn request...");
            }
        }
    }

    void RcfServer::spawnReverseConnection(int count, const std::string &ip, int port, const std::string &endpointName, const std::string &endpointPassword)
    {
        RCF_TRACE("")(count)(ip)(port)(endpointName)(endpointPassword);

        if (endpointName == this->endpointName && 
            endpointPassword == this->endpointPassword &&
            count > 0 && count < 20)
        {
            for (int i=0; i<count; i++)
            {
                RcfClient<I_EndpointBroker> endpointBroker(ip, port);
                endpointBroker.setOneway(true);
                endpointBroker.reverseThisConnection(endpointName, endpointPassword);
                getMultiplexer().addFd( endpointBroker.releaseConnection()->releaseFd() );
            }
        }
        else
        {
            RCF_TRACE("refusing to spawn connections")(endpointName)(this->endpointName)(endpointPassword)(this->endpointPassword)(count);
        }
    }
    
} // namespace RCF
