#include "stdafx.h"
#include "scheduler.h"


void scheduler::start()
{
    iocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, scheds);
    if (NULL == iocp)
    {
        throw (int)::WSAGetLastError();
    }
}

void scheduler::stop()
{
}

void scheduler::push(client * clt)
{
    ::PostQueuedCompletionStatus(iocp, 0, (ULONG_PTR)clt, NULL);
}
