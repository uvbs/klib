#include "stdafx.h"
#include "processor.h"

#include "scheduler.h"
#include "client.h"


void processor::start()
{
    for (int i = 0;i < threads;i ++)
    {
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
    }
}

void processor::stop()
{
}

DWORD WINAPI processor::run(LPVOID param)
{
    processor& procor = *(processor *)param;
    scheduler& scheder = *procor.scheder;
    HANDLE iocp = scheder.iocp;

    DWORD ready;
    ULONG_PTR key;
    WSAOVERLAPPED * overlap;
    while (true)
    {
        ::GetQueuedCompletionStatus(iocp, &ready, &key, (LPOVERLAPPED *)&overlap, INFINITE);

        client * clt = (client *)key;
        switch(clt->event)
        {
        case client::EV_RECV:
            {
                if (0 >= ready)
                {
                    clt->event = client::EV_DISCONNECT;
                    ::PostQueuedCompletionStatus(iocp, 0, (ULONG_PTR)clt, NULL);
                }
                else
                {
                    clt->on_recved(nullptr, ready);
                }
            }
            break;
        case client::EV_CONNECT:
            {
                if (NULL == ::CreateIoCompletionPort((HANDLE)clt->fd, iocp, (ULONG_PTR)clt, 0))
                {
                    ::closesocket(clt->fd);
                    delete clt;
                }
                else
                {
                    clt->OnConnect();
                }
            }
            break;
        case client::EV_DISCONNECT:
            {
                clt->OnDisconnect();
                ::closesocket(clt->fd);
                delete clt;
            }
            break;
        case client::EV_SEND:
            break;
        }
    }

    return 0;
}
