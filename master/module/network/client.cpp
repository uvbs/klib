#include "stdafx.h"
#include "client.h"

void client::send(const char * buff, int len)
{
    ::send(fd, buff, len, 0);
}

void client::recv(char * buff, int len)
{
    this->event = EV_RECV;

    WSAOVERLAPPED overlap;
    ::memset(&overlap, 0, sizeof(overlap));
    WSABUF buf;
    buf.buf = buff;
    buf.len = len;
    DWORD ready = 0;
    DWORD flags = 0;
    if (0 != ::WSARecv(fd, &buf, 1, &ready, &flags, &overlap, NULL)
        && WSA_IO_PENDING != WSAGetLastError())
    {
        this->event = EV_DISCONNECT;
        ::PostQueuedCompletionStatus(iocp, 0, (ULONG_PTR)this, NULL);
    }
}

void client::close()
{
    ::shutdown(fd, SD_BOTH);
}

bool client::on_recved(const char* buff, uint32_t len) // 接收到数据的回调
{
    return true;
}

bool client::on_sended(const char* buff, uint32_t len) // 发送成功的回调
{
    return true;
}
void client::OnConnect(){}
void client::OnDisconnect(){}