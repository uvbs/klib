
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _RCF_CLIENTSTUB_HPP_
#define _RCF_CLIENTSTUB_HPP_

#include <string>
#include <memory>

#include <RCF/Token.hpp>
#include <RCF/Protocol/Protocol.hpp>

namespace RCF {

    class Connection;

    extern const int DefaultClientRecvTimeoutMs;

    class ClientStub
    {
    public:
        ClientStub();
        ClientStub(
            const std::string &ip, 
            int port, 
            int fd, 
            bool oneway,
            bool service,
            unsigned int recvTimeoutMs,
            const std::string &endpointName, 
            const std::string &objectName, 
            const std::string &interfaceName);
        ClientStub(const ClientStub &rhs);
        ClientStub &operator=(const ClientStub &rhs);
        virtual ~ClientStub();
        
        // TODO: a lot of these get/set functions should not be publicly available
        Token getToken() const;
        void setToken(Token token);

        std::string getObjectName() const { return objectName; }
        void setObjectName(const std::string &objectName) { this->objectName = objectName; }
        
        bool getOneway() const { return oneway; }
        void setOneway(bool oneway) { this->oneway = oneway; } // TODO: oneway should be immutable
        
        bool getService() const { return service; }
        void setService(bool service) { this->service = service; }

        unsigned int getRecvTimeoutMs() const { return recvTimeoutMs; }
        void setRecvTimeoutMs(unsigned int recvTimeoutMs) { this->recvTimeoutMs = recvTimeoutMs; }
        
        std::string getIp() const { return ip; }
        void setIp(const std::string &ip) { this->ip = ip; }

        int getPort() const { return port; }
        void setPort(int port) { this->port = port; }

        void setFd(int fd) { this->fd = fd; }
        int getFd() const { return fd; }

        void setProtocol(int protocol) { this->protocol = protocol; }
        int getProtocol() { return protocol; }

        Connection& getConnection();
        void setConnection( std::auto_ptr<Connection> connection );
        std::auto_ptr<Connection> releaseConnection();
        bool isConnected();

        template<typename Archive> void serialize(Archive &ar, const unsigned int)
        {
            ar & token
               & ip
               & port
               & oneway 
               & service 
               & recvTimeoutMs 
               & protocol
               & endpointName 
               & objectName 
               & interfaceName;
        }

    private:
        Token token;
        std::string ip;
        int port;
        int fd;
        bool oneway;
        bool service;
        unsigned int recvTimeoutMs;
        int protocol;
        std::string endpointName;
        std::string objectName;
        std::string interfaceName;
        std::auto_ptr<Connection> connection;
    };

} // namespace RCF

#endif // ! _RCF_CLIENTSTUB_HPP_
