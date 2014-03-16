
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _RCF_CONNECTION_HPP_
#define _RCF_CONNECTION_HPP_

#include <string>
#include <sstream>

// Serialization protocols

#ifndef RCF_NO_SF_SERIALIZATION
#include <RCF/Protocol/SF.hpp>
#endif

#ifndef RCF_NO_BOOST_SERIALIZATION
#include <RCF/Protocol/BoostSerialization.hpp>
#endif

namespace RCF {

    inline bool isProtocolSupported(int protocol)
    {
        switch (protocol)
        {
        case 1: return Protocol<1>::getName() != "";
        case 2: return Protocol<2>::getName() != "";
        case 3: return Protocol<3>::getName() != "";
        case 4: return Protocol<4>::getName() != "";
        case 5: return Protocol<5>::getName() != "";
        case 6: return Protocol<6>::getName() != "";
        case 7: return Protocol<7>::getName() != "";
        case 8: return Protocol<8>::getName() != "";
        case 9: return Protocol<9>::getName() != "";
        case 10: return Protocol<10>::getName() != "";
        default: return false;
        }
    }

    inline std::string getProtocolName(int protocol)
    {
        switch (protocol)
        {
        case 1: return Protocol<1>::getName();
        case 2: return Protocol<2>::getName();
        case 3: return Protocol<3>::getName();
        case 4: return Protocol<4>::getName();
        case 5: return Protocol<5>::getName();
        case 6: return Protocol<6>::getName();
        case 7: return Protocol<7>::getName();
        case 8: return Protocol<8>::getName();
        case 9: return Protocol<9>::getName();
        case 10: return Protocol<10>::getName();
        default: return "";
        }
    }

} // namespace RCF

namespace RCF {

    class Socket
    {
    public:
        Socket(const std::string &ip, int port);
        Socket(int fd);
        ~Socket();

        int getFd();
        int releaseFd() { own = false; return fd; }

        void close();
        bool isConnected();
        
    private:
        bool own;
        int fd;
        std::string ip;
        int port;
    };
    
    class PointerContext
    {
    public:
        template<typename SmartPtr, typename T> 
        SmartPtr get(T t)
        {
            return static_cast<SmartPtr>( ptrMap[&typeid(SmartPtr)][ static_cast<void *>(t) ] );
        }

        template<typename SmartPtr, typename T> 
        void set(SmartPtr sp, T t)
        {
            ptrMap[&typeid(SmartPtr)][ static_cast<void *>(t) ] = static_cast<void *>(sp);
        }

        void clear()
        {
            ptrMap.clear();
        }

    private:
        std::map< const std::type_info *, std::map<void*, void*> > ptrMap;
    };


    class Token;
    class MethodInvocationRequest;
    class MethodInvocationResponse;

    class Connection 
    {
    public:
        Connection(std::string ip, int port, unsigned int timeoutMs, int protocol = DefaultProtocol);
        Connection(int fd, unsigned int timeoutMs, int protocol = DefaultProtocol);

        template<typename T> void write(const T &t);
        template<typename T> void read(T &t);

        void prepareSend();
        int send();
        int receive();
        void close();
        bool isConnected();
        
        unsigned int getRecvTimeoutMs() const { return recvTimeoutMs; }
        void setRecvTimeoutMs(unsigned int recvTimeoutMs) { this->recvTimeoutMs = recvTimeoutMs; }
        
        int releaseFd() { return sock.releaseFd(); }

        int getProtocol() { return protocol; }
        void setProtocol(int protocol) { this->protocol = protocol; }

        PointerContext &getPointerContext() { return pointerContext; }

    private:
        int protocol;

        std::istringstream ibuffer;
        std::ostringstream obuffer;

        Protocol<1>::In iprotocol1;
        Protocol<1>::Out oprotocol1;

        Protocol<2>::In iprotocol2;
        Protocol<2>::Out oprotocol2;

        Protocol<3>::In iprotocol3;
        Protocol<3>::Out oprotocol3;

        Protocol<4>::In iprotocol4;
        Protocol<4>::Out oprotocol4;

        Protocol<5>::In iprotocol5;
        Protocol<5>::Out oprotocol5;

        Protocol<6>::In iprotocol6;
        Protocol<6>::Out oprotocol6;

        Protocol<7>::In iprotocol7;
        Protocol<7>::Out oprotocol7;

        Protocol<8>::In iprotocol8;
        Protocol<8>::Out oprotocol8;

        Protocol<9>::In iprotocol9;
        Protocol<9>::Out oprotocol9;

        Protocol<10>::In iprotocol10;
        Protocol<10>::Out oprotocol10;

        void bindInProtocol();
        void unbindInProtocol();
        void bindOutProtocol();
        void unbindOutProtocol();

        void encodeProtocol();
        void decodeProtocol();

        int recv(unsigned int timeoutMs, int fd, char *buffer, int length, int flags);
        
        Socket sock;
        PointerContext pointerContext;
        unsigned int recvTimeoutMs;
    };

    template<typename T> Connection &operator<<(Connection &connection, const T &t);
    template<typename T> Connection &operator>>(Connection &connection,       T &t);

} // namespace RCF

#include <RCF/Connection.inl>

#endif // ! _RCF_CONNECTION_HPP_
