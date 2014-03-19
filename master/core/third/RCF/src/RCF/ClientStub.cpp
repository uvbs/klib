
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

#include <RCF/ClientStub.hpp>

#include <RCF/Connection.hpp>
#include <RCF/MethodInvocation.hpp>
#include <RCF/Protocol/Protocol.hpp>
#include <RCF/ServerInterfaces.hpp>
#include <RCF/Tools.hpp>

namespace RCF {

    //****************************************
    // ClientStub

    const int DefaultClientRecvTimeoutMs = 1000*10;  // 10s

    ClientStub::ClientStub() : 
        token(),
        ip(),
        port(),
        fd(-1),
        oneway(),
        service(),
        recvTimeoutMs(DefaultClientRecvTimeoutMs),
        protocol(DefaultProtocol),
        endpointName(),
        objectName(),
        interfaceName()
    {}

    ClientStub::ClientStub(const ClientStub &rhs) : 
        token(rhs.token),
        ip(rhs.ip),
        port(rhs.port),
        fd(-1),
        oneway(rhs.oneway),
        service(rhs.service),
        recvTimeoutMs(rhs.recvTimeoutMs),
        protocol(rhs.protocol),
        endpointName(rhs.endpointName),
        objectName(rhs.objectName),
        interfaceName(rhs.interfaceName)
    {}

    ClientStub &ClientStub::operator=( const ClientStub &rhs ) 
    {
        if (&rhs != this) 
        {
            if (interfaceName == rhs.interfaceName)
            {
                token = rhs.token;
                ip = rhs.ip;
                port = rhs.port;
                fd = -1;
                oneway = rhs.oneway;
                service = rhs.service;
                recvTimeoutMs = rhs.recvTimeoutMs;
                protocol = rhs.protocol;
                endpointName = rhs.endpointName;
                objectName = rhs.objectName;
            }
            else {
                RCF_THROW(RCF::Exception, "incompatible client stub assignment")(interfaceName)(rhs.interfaceName);
            }
        }
        return *this;
    }

    ClientStub::~ClientStub()
    {
        if (connection.get())
            (*connection) << MethodInvocationRequest(true); // TODO: actually send it!!!
    }

    Token ClientStub::getToken() const 
    { 
        return token; 
    }
    
    void ClientStub::setToken(Token token) 
    { 
        this->token = token; 
    }

    void ClientStub::setConnection( std::auto_ptr<Connection> connection ) 
    { 
        this->connection = connection; 
    }
    
    std::auto_ptr<Connection> ClientStub::releaseConnection() 
    { 
        return connection;
    }

    ClientStub::ClientStub(
        const std::string &ip, 
        int port, 
        int fd, 
        bool oneway, 
        bool service,
        unsigned int recvTimeoutMs,
        const std::string &endpointName, 
        const std::string &objectName, 
        const std::string &interfaceName) :
            ip(ip),
            port(port),
            fd(fd),
            oneway(oneway),
            service(service),
            recvTimeoutMs(recvTimeoutMs),
            protocol(DefaultProtocol),
            endpointName(endpointName),
            objectName(objectName),
            interfaceName(interfaceName)
    {}

    Connection& ClientStub::getConnection() 
    { 
        if (connection.get() && !connection->isConnected())
        {
            RCF_TRACE("Server dropped connection... connecting again")(ip)(port);
            connection.reset();
        }
        
        if (NULL == connection.get()) 
        {
            if (fd == -1 && ip.empty())
            {
                RCF_THROW( RCF::ConnectionException, "not enough information to establish connection" )(fd)(ip)(port);
            }
            if (service || token != Token())
            {
                connection = 
                    fd == -1 ?
                    std::auto_ptr<Connection>( new Connection(ip, port, recvTimeoutMs) ) :
                    std::auto_ptr<Connection>( new Connection(fd, recvTimeoutMs) );
            }
            else
            {
                if (!endpointName.empty())
                {
                    RcfClient<I_EndpointBroker> endpointBroker(ip, port);
                    if (fd != -1)
                    {
                        endpointBroker.setFd(fd);
                    }
                    endpointBroker.bindToEndpoint(endpointName);
                    RcfClient<I_ObjectFactory> factory;
                    factory.setConnection( endpointBroker.releaseConnection() );
                    factory.setService(true);
                    setToken( factory.make(objectName) );
                    setConnection( factory.releaseConnection() );
                }
                else
                {
                    RcfClient<I_ObjectFactory> factory(ip, port);
                    if (fd != -1)
                    {
                        factory.setFd(fd);
                    }
                    factory.setService(true);
                    factory.setRecvTimeoutMs( getRecvTimeoutMs() );
                    factory.setProtocol( getProtocol() );
                    Token token = factory.make(objectName);
                    setToken(token);
                    setConnection( factory.releaseConnection() );
                }
            }
        }
        RCF_ASSERT( connection.get() );

        connection->setRecvTimeoutMs( getRecvTimeoutMs() );

        return *connection; 
    }

    bool ClientStub::isConnected()
    {
        return connection.get() && connection->isConnected();
    }

}
