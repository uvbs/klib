
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

#include <RCF/Connection.hpp>

#include <RCF/ByteOrdering.hpp>
#include <RCF/MethodInvocation.hpp>
#include <RCF/Token.hpp>
#include <RCF/util/Platform/Platform.hpp>
#include <RCF/util/Platform/OS/SocketInit.hpp>

#include <sys/timeb.h>
#include <sys/types.h>

namespace RCF {

    //******************************************************
    // nonblocking socket routines

    // time in ms since ca 1970, modulo 65536 s (ca 18.2 hrs)
    int getCurrentTimeMs()
    {
        struct _timeb timebuffer;
        _ftime( &timebuffer );
        int timeS = static_cast<int>(timebuffer.time);
        int timeMs = timebuffer.millitm;
        return (timeS & 0xffff)*1000 + timeMs;
    }

    int timedConnect(int timeoutMs, int fd, sockaddr *addr, int size)
    {
        int startTimeMs = getCurrentTimeMs();
        int endTimeMs = startTimeMs + timeoutMs;
        int ret = Platform::OS::Socket::connect(fd, addr, size);
        if ((ret == -1 && 
            Platform::OS::Socket::GetLastError() == Platform::OS::Socket::ERR_EWOULDBLOCK)
            ||
            (ret == -1 &&
            Platform::OS::Socket::GetLastError() == Platform::OS::Socket::ERR_EINPROGRESS))
        {
            fd_set fdSet;
            FD_ZERO(&fdSet);
            FD_SET(fd, &fdSet);
            int timeoutMs = endTimeMs - getCurrentTimeMs();
            timeoutMs = (timeoutMs > 0) ? timeoutMs : 0;
            timeval timeout;
            timeout.tv_sec = timeoutMs/1000;
            timeout.tv_usec = 1000*(timeoutMs%1000);
            if (timeout.tv_usec < 0)
            {
                timeout.tv_usec = 0;
            }
            ret = Platform::OS::Socket::select(fd+1, NULL, &fdSet, NULL, &timeout);
            if (ret == 1)
            {
                return 0;
            }
            else if (ret == 0)
            {
                RCF_TRACE("timedConnect - timeout")(fd)(timeout.tv_sec)(timeout.tv_usec);
                return -1;
            }
            else
            {
                int err = Platform::OS::Socket::GetLastError();
                RCF_TRACE("timedConnect - error")(fd)(timeout.tv_sec)(timeout.tv_usec)(err)(Platform::OS::GetErrorString(err));
                return -1;
            }
        }
        else if (ret == 0)
        {
            return 0;
        }
        else
        {
            int err = Platform::OS::Socket::GetLastError();
            RCF_TRACE("timedConnect - error")(fd)(err)(Platform::OS::GetErrorString(err));
            return -1;
        }
        
    }

    int timedRecv(int timeoutMs, int fd, char *buffer, int length, int flags)
    {
        int startTimeMs = getCurrentTimeMs();
        int endTimeMs = startTimeMs + timeoutMs;
        int bytesRead = 0;
        int bytesRemaining = length;
        int ret = 0;
        int err = 0;
        
        while (true)
        {
            ret = Platform::OS::Socket::recv(fd, buffer, bytesRemaining, flags);
            err = Platform::OS::Socket::GetLastError();
            if (ret > 0)
            {
                buffer += ret;
                bytesRead += ret;
                bytesRemaining -= ret;
                RCF_ASSERT(bytesRead >= 0 && bytesRemaining >= 0)(bytesRead)(bytesRemaining)(ret);
                if (bytesRemaining > 0) 
                {
                    continue;
                }
                else if (bytesRemaining == 0)
                {
                    return bytesRead;
                }
            }
            else if (ret == 0)
            {
                return 0;
            }
            else if (
                ret == -1 && 
                err == Platform::OS::Socket::ERR_EWOULDBLOCK &&
                bytesRemaining > 0)
            {
                int timeoutMs = endTimeMs - getCurrentTimeMs();
                timeoutMs = (timeoutMs > 0) ? timeoutMs : 0;
                fd_set fdSet;
                FD_ZERO(&fdSet);
                FD_SET(fd, &fdSet);
                timeval timeout;
                timeout.tv_sec = timeoutMs/1000;
                timeout.tv_usec = 1000*(timeoutMs%1000);
                ret = Platform::OS::Socket::select(fd+1, &fdSet, NULL, NULL, &timeout);
                RCF_ASSERT(ret == -1 || ret == 0 || ret == 1)(ret);
                if (ret == 1)
                {
                    continue;
                }
                else if (ret == 0)
                {
                    RCF_TRACE("timedRecv - select() timeout")(fd)(timeout.tv_sec)(timeout.tv_usec);
                    return -2;
                }
                else if (ret == -1)
                {
                    err = Platform::OS::Socket::GetLastError();
                    RCF_TRACE("timedRecv - select() error")(fd)(timeout.tv_sec)(timeout.tv_usec)(ret)(err)(Platform::OS::GetErrorString(err));
                    return -1;
                }
            }
            else if (ret == -1)
            {
                err = Platform::OS::Socket::GetLastError();
                RCF_TRACE("timedRecv - select() error")(fd)(ret)(err)(Platform::OS::GetErrorString(err));
                return -1;
            }
        }
    }

    int timedSend(int timeoutMs, int fd, const char *buffer, int length, int flags)
    {
        // TODO
        int startTimeMs = getCurrentTimeMs();
        int endTimeMs = startTimeMs + timeoutMs;
        int bytesRead = 0;
        int bytesRemaining = length;
        int ret = 0;
        int err = 0;
        while (true)
        {
            ret = Platform::OS::Socket::send(fd, buffer, bytesRemaining, flags);
            err = Platform::OS::Socket::GetLastError();

            if (ret > 0)
            {
                buffer += ret;
                bytesRead += ret;
                bytesRemaining -= ret;
                RCF_ASSERT(bytesRead >= 0 && bytesRemaining >= 0)(bytesRead)(bytesRemaining)(ret);
                if (bytesRemaining > 0) 
                {
                    continue;
                }
                else if (bytesRemaining == 0)
                {
                    return bytesRead;
                }
            }
            else if (ret == 0)
            {
                RCF_ASSERT(0); // can this ever happen?
            }
            else if (
                ret == -1 && 
                err == Platform::OS::Socket::ERR_EWOULDBLOCK &&
                bytesRemaining > 0)
            {
                int timeoutMs = endTimeMs - getCurrentTimeMs();
                timeoutMs = (timeoutMs > 0) ? timeoutMs : 0;
                fd_set fdSet;
                FD_ZERO(&fdSet);
                FD_SET(fd, &fdSet);
                timeval timeout;
                timeout.tv_sec = timeoutMs/1000;
                timeout.tv_usec = 1000*(timeoutMs%1000);
                ret = Platform::OS::Socket::select(fd+1, NULL, &fdSet, NULL, &timeout);
                RCF_ASSERT(ret == -1 || ret == 0 || ret == 1)(ret);
                if (ret == 1)
                {
                    continue;
                }
                else if (ret == 0)
                {
                    RCF_TRACE("timedSend - select() timeout")(fd)(timeout.tv_sec)(timeout.tv_usec);
                    return -2;
                }
                else if (ret == -1)
                {
                    err = Platform::OS::Socket::GetLastError();
                    RCF_TRACE("timedSend - select() error")(fd)(timeout.tv_sec)(timeout.tv_usec)(ret)(err)(Platform::OS::GetErrorString(err));
                    return -1;
                }
            }
            else if (ret == -1)
            {
                err = Platform::OS::Socket::GetLastError();
                RCF_TRACE("timedSend - send() error")(fd)(ret)(err)(Platform::OS::GetErrorString(err));
                return -1;
            }
        }
    }

    
    //******************************************************
    // Socket

    Socket::Socket(const std::string &ip, int port) 
        : own(true), fd(-1), ip(ip), port(port)
    {}

    Socket::Socket(int fd) 
        : own(false), fd(fd), ip(), port()
    {}

    Socket::~Socket()
    {
        if (own && fd != -1)
        {
            close();
        }
    }

    int Socket::getFd()
    {
        if (fd == -1)
        {
            // establish a connection
            hostent *hostDesc = ::gethostbyname( ip.c_str() );
            std::string strIp = ::inet_ntoa( * (in_addr*) hostDesc->h_addr_list[0]);
            fd = static_cast<int>( ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP) );
            if (fd < 0)
            {
                RCF_THROW(RCF::ConnectionException, "unable to create socket")(fd);
            }
            Platform::OS::Socket::setblocking(fd, false);
            sockaddr_in serverAddr;
            memset(&serverAddr, 0, sizeof(serverAddr));
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_addr.s_addr = ::inet_addr(strIp.c_str());
            //serverAddr.sin_port = ::htons(port); // the :: seems to screw up gcc (!?!)
            serverAddr.sin_port = htons(port); // the :: seems to screw up gcc (!?!)
            unsigned int timeoutMs = 1000;
            if (0 != timedConnect(timeoutMs, fd, (sockaddr*) &serverAddr, sizeof(serverAddr))) 
            {
                int err = Platform::OS::Socket::closesocket(fd);
                RCF_ASSERT( err == 0 );
                fd = -1;
                RCF_THROW(RCF::ConnectionException, "timed connection attempt failed")(strIp)(ip)(port);
            }
        }
        RCF_ASSERT(fd != -1);
        return fd;
    }

    void Socket::close()
    {
        int err = Platform::OS::Socket::closesocket(fd);
        if (err < 0)
        {
            int socket_err = Platform::OS::Socket::GetLastError();
            RCF_ASSERT(0)(fd)(err)(socket_err)(Platform::OS::GetErrorString(socket_err));
        }
        fd = -1;
        
    }
    
    bool Socket::isConnected()
    {
        if (fd == -1)
        {
            return false;
        }
        else
        {
            timeval tv = {0,0};
            fd_set readFds;
            FD_ZERO(&readFds);
            FD_SET(fd, &readFds);
            int ret = Platform::OS::Socket::select(fd+1, &readFds, NULL, NULL, &tv);
            if (ret == 1)
            {
                const int length = 1;
                char buffer[length];
                int ret = Platform::OS::Socket::recv(fd, buffer, length, MSG_PEEK);
                if (ret == 0)
                {
                    return false;
                }
                else if (ret == -1)
                {
                    ret = Platform::OS::Socket::GetLastError();
                    if (ret == Platform::OS::Socket::ERR_ECONNRESET || 
                        ret == Platform::OS::Socket::ERR_ECONNABORTED || 
                        ret == Platform::OS::Socket::ERR_ECONNREFUSED)
                    {
                        return false;
                    }
                }
            }
            return true;
        }
    }

    //******************************************************
    // Connection

    Connection::Connection(std::string ip, int port, unsigned int recvTimeoutMs, int protocol) :
        protocol(protocol),
        sock(ip, port),
        recvTimeoutMs(recvTimeoutMs)
    {
        RCF_TRACE("")(ip)(port);
    }

    Connection::Connection(int fd, unsigned int recvTimeoutMs, int protocol) :
        protocol(protocol),
        sock(fd),
        recvTimeoutMs(recvTimeoutMs)
    {
        RCF_TRACE("")(fd);
    }

    int Connection::recv(unsigned int timeoutMs, int fd, char *buffer, int length, int flags)
    {
        return timedRecv(timeoutMs, fd, buffer, length, flags);

        //Platform::OS::Socket::setblocking(fd, true);
        //int ret = ::recv(fd, buffer, length, flags);
        //Platform::OS::Socket::setblocking(fd, false);
        //return ret;
    }

    int Connection::receive()
    {
        RCF_TRACE("");

        int fd = sock.getFd();
        unsigned int length = 0;
        unsigned int timeout = recvTimeoutMs;
        int ret = recv(timeout, fd, (char *) &length, 4, 0);
        if (ret == 4)
        {
            Reorder<NetworkByteOrder, MachineByteOrder>()(&length, sizeof(length), 1);
            if (length > 1024*50) // 50K limit on message sizes, for now
            {
                RCF_THROW(RecvException, "too much data")(fd)(length);
            }
            std::vector<char> vec(length);
            ret = recv(timeout, fd, &vec[0], length, 0);
            if (ret == length)
            {
                std::string data(&vec[0], length);
                unbindInProtocol(); // TODO: necessary to do explicit unbind?
                ibuffer.str(data);
                ibuffer.clear();
                decodeProtocol();
                bindInProtocol();
                return ret;
            }
        }

        RCF_ASSERT( ret >= -2);
        if (ret == 0)
        {
            RCF_TRACE("Connection::receive - graceful close")(fd)(ret);
            return 0;
        }
        else if (ret == -1)
        {
            int socket_err = Platform::OS::Socket::GetLastError();
            RCF_TRACE("Connection::receive - error")(fd)(ret)(socket_err)(Platform::OS::GetErrorString(socket_err));
            return -1;
        }
        else if (ret == -2)
        {
            return -2; // time out
        }
        else //if (ret > 0)
        {
            RCF_TRACE("Connection::receive - not enough data")(fd)(ret);
            return -1;
        }
    }

    void Connection::close()
    {
        sock.close();
    }

    bool Connection::isConnected()
    {
        return sock.isConnected();
    }

    void Connection::prepareSend()
    {
        unbindOutProtocol(); // TODO: necesary to do explicit unbind?
        obuffer.str("");
        ibuffer.clear();
        encodeProtocol();
        bindOutProtocol();
    }

    int Connection::send()
    {
        RCF_TRACE("");

        int fd = sock.getFd();
        std::string packet = obuffer.str();
        int length = static_cast<int>(packet.length());
        Reorder<MachineByteOrder, NetworkByteOrder>()(&length, sizeof(length), 1);
        std::string data((char *) &length, 4);
        data.append(packet);

        unsigned int sendTimeoutMs = getRecvTimeoutMs() / 4;
        if (sendTimeoutMs < 0)
            sendTimeoutMs = 250;
        if (sendTimeoutMs > 5000)
            sendTimeoutMs = 5000;
        int err = timedSend(sendTimeoutMs, fd, data.c_str(), static_cast<int>(data.length()), 0);
        if (err != data.length())
        {
            int socket_err = Platform::OS::Socket::GetLastError();
            RCF_THROW(RCF::SendException, "timed send failure")(sendTimeoutMs)(fd)(err)(socket_err)(Platform::OS::GetErrorString(socket_err));
        }


        RCF_TRACE("")(data)(data.length())(fd);

        return static_cast<int>(data.length());
    }

    void Connection::encodeProtocol()
    {
        int protocol = getProtocol();
        if (protocol < 0 || protocol > 15)
        {
            RCF_THROW(RCF::SerializationException, "bad protocol number")(protocol);
        }
        char ch = static_cast<char>(protocol);
        obuffer << ch;
    }

    void Connection::decodeProtocol()
    {
        char ch = 0;
        ibuffer >> ch;
        int protocol = static_cast<int>(ch);
        if (protocol < 0 || protocol > 15)
        {
            RCF_THROW(RCF::SerializationException, "bad protocol number")(protocol);
        }
        setProtocol(protocol);
    }

    void Connection::bindInProtocol()
    {
        switch (protocol)
        {
        case 1: iprotocol1.bind(ibuffer); break;
        case 2: iprotocol2.bind(ibuffer); break;
        case 3: iprotocol3.bind(ibuffer); break;
        case 4: iprotocol4.bind(ibuffer); break;
        case 5: iprotocol5.bind(ibuffer); break;
        case 6: iprotocol6.bind(ibuffer); break;
        case 7: iprotocol7.bind(ibuffer); break;
        case 8: iprotocol8.bind(ibuffer); break;
        case 9: iprotocol9.bind(ibuffer); break;
        case 10: iprotocol10.bind(ibuffer); break;
        default: RCF_ASSERT(0);
        }
        getPointerContext().clear();
    }

    void Connection::unbindInProtocol()
    {
        switch (protocol)
        {
        case 1: iprotocol1.unbind(); break;
        case 2: iprotocol2.unbind(); break;
        case 3: iprotocol3.unbind(); break;
        case 4: iprotocol4.unbind(); break;
        case 5: iprotocol5.unbind(); break;
        case 6: iprotocol6.unbind(); break;
        case 7: iprotocol7.unbind(); break;
        case 8: iprotocol8.unbind(); break;
        case 9: iprotocol9.unbind(); break;
        case 10: iprotocol10.unbind(); break;
        default: RCF_ASSERT(0);
        }
    }

    void Connection::bindOutProtocol()
    {
        switch (protocol)
        {
        case 1: oprotocol1.bind(obuffer); break;
        case 2: oprotocol2.bind(obuffer); break;
        case 3: oprotocol3.bind(obuffer); break;
        case 4: oprotocol4.bind(obuffer); break;
        case 5: oprotocol5.bind(obuffer); break;
        case 6: oprotocol6.bind(obuffer); break;
        case 7: oprotocol7.bind(obuffer); break;
        case 8: oprotocol8.bind(obuffer); break;
        case 9: oprotocol9.bind(obuffer); break;
        case 10: oprotocol10.bind(obuffer); break;
        default: RCF_ASSERT(0);
        }
    }

    void Connection::unbindOutProtocol()
    {
        switch (protocol)
        {
        case 1: oprotocol1.unbind(); break;
        case 2: oprotocol2.unbind(); break;
        case 3: oprotocol3.unbind(); break;
        case 4: oprotocol4.unbind(); break;
        case 5: oprotocol5.unbind(); break;
        case 6: oprotocol6.unbind(); break;
        case 7: oprotocol7.unbind(); break;
        case 8: oprotocol8.unbind(); break;
        case 9: oprotocol9.unbind(); break;
        case 10: oprotocol10.unbind(); break;
        default: RCF_ASSERT(0);
        }
    }

}
