#include "StdAfx.h"
#include "INetConnection.h"

INetConnection::INetConnection(void)
{
    datalen = 0;
    m_PostReadCount = 0;
    m_PostWriteCount = 0;
    m_bClosing = FALSE;
    m_tLastActive = 0;
    m_PeerPort = 0;
    m_LocalPort = 0;
    bind_key_ = 0;
    m_dwBytesRead = 0;
    m_dwBytesWrite = 0;
    memset(m_strAddress, 0, sizeof(m_strAddress));
}

INetConnection::~INetConnection(void)
{
    datalen = 0;
    m_PostReadCount = 0;
    m_PostWriteCount = 0;
    m_bClosing = FALSE;
    m_tLastActive = 0;
    m_PeerPort = 0;
    m_LocalPort = 0;
    bind_key_ = 0;
    m_dwBytesRead = 0;
    m_dwBytesWrite = 0;
    memset(m_strAddress, 0, sizeof(m_strAddress));
}

bool INetConnection::init_peer_info()
{
    sockaddr_in sockaddr;
    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    int len = sizeof(sockaddr);
    if (SOCKET_ERROR != getpeername(get_socket(), (struct sockaddr*) & sockaddr, &len)) {
        set_peer_port(ntohs(sockaddr.sin_port));
        m_PeerAddr = sockaddr.sin_addr.s_addr;
        return true;
    }

    return false;
}

// char* INetConnection::get_peer_addr() 
// {
// 	if (m_strAddress[0] == '\0') {
// 		set_peer_addr(inet_ntoa(*(in_addr*) &m_PeerAddr));
// 	}
// 	return m_strAddress;
// }

bool INetConnection::write_recv_stream(const char* buff, size_t len) 
{
    recv_stream_.write((net_stream_type::value_type*) buff, len);
    return true;
}

bool INetConnection::read_recv_stream(char* buff, size_t len)
{
    if (recv_stream_.size() >= len) 
    {
        recv_stream_.read((net_stream_type::value_type*) buff, len);
        return true;
    }

    return false;
}

size_t INetConnection::get_recv_length()
{
    return recv_stream_.size();
}

void INetConnection::UpdateLastActiveTimestamp()  
{
    m_tLastActive = GetTickCount();
}

void INetConnection::AddPostReadCount() 
{
    guard helper(mutex_);
    ++ m_PostReadCount;
}

void INetConnection::DecPostReadCount()
{
    guard helper(mutex_);
    -- m_PostReadCount;
}

USHORT INetConnection::GetPostReadCount()
{
    guard helper(mutex_);
    return m_PostReadCount;
}

void INetConnection::AddPostWriteCount()
{
    guard helper(mutex_);
    ++ m_PostWriteCount;
}

void INetConnection::DecPostWriteCount()
{
    guard helper(mutex_);
    -- m_PostWriteCount;
}

USHORT INetConnection::GetPostWriteCount()
{
    guard helper(mutex_);
    return m_PostWriteCount;
}
