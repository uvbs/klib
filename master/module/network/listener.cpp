#include "stdafx.h"
#include "listener.h"
#include "scheduler.h"
#include "client.h"

void listener::start()
{
    SOCKET fd = INVALID_SOCKET;

    try
    {
        fd = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
        if (INVALID_SOCKET == fd)
        {
            throw (int)WSAGetLastError();
        }

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = ip;
        addr.sin_port = htons(port);
        if (SOCKET_ERROR == ::bind(fd, (struct sockaddr *)&addr, sizeof(addr)))
        {
            throw (int)::WSAGetLastError();
        }

        if (SOCKET_ERROR == ::listen(fd, backlogs))
        {
            throw (int)::WSAGetLastError();
        }

        DWORD tid;
        HANDLE thd = ::CreateThread(NULL,
            0,
            (LPTHREAD_START_ROUTINE)run,
            this,
            0,
            &tid);
        if (NULL == thd)
        {
            throw (int)::GetLastError();
        }
        ::CloseHandle(thd);

        this->fd = fd;
    }
    catch(...)
    {
        if (INVALID_SOCKET != fd)
        {
            ::closesocket(fd);
        }

        throw;
    }
}

void listener::stop()
{
}

DWORD WINAPI listener::run(LPVOID param)
{
    listener& the_listener = *(listener *)param;
    scheduler& scheder = *(scheduler *)the_listener.scheder;
    SOCKET lsfd = the_listener.fd;
    HANDLE iocp = scheder.iocp;

    SOCKET fd;
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);
    while (true)
    {
        fd = ::WSAAccept(lsfd, (struct sockaddr *)&addr, &addrlen, NULL, NULL);
        if (INVALID_SOCKET == fd)
        {
            break;
        }

        client * clt = new client();
        clt->iocp = iocp;
        clt->fd = fd;
        clt->ip = addr.sin_addr.s_addr;
        clt->port = htons(addr.sin_port);
        clt->event = client::EV_CONNECT;
        clt->srv = the_listener.srv;
        scheder.push(clt);
    }

    return 0;
}
