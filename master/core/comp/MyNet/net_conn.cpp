#include "StdAfx.h"
#include "net_conn.h"

net_conn::net_conn(void)
{
    datalen = 0;
    post_read_count_ = 0;
    post_write_count_ = 0;
    is_closing_ = FALSE;
    last_active_tm_ = 0;
    peer_port_ = 0;
    local_port_ = 0;
    bind_key_ = 0;
    m_dwBytesRead = 0;
    m_dwBytesWrite = 0;
    memset(m_strAddress, 0, sizeof(m_strAddress));
}

net_conn::~net_conn(void)
{
    datalen = 0;
    post_read_count_ = 0;
    post_write_count_ = 0;
    is_closing_ = FALSE;
    last_active_tm_ = 0;
    peer_port_ = 0;
    local_port_ = 0;
    bind_key_ = 0;
    m_dwBytesRead = 0;
    m_dwBytesWrite = 0;
    memset(m_strAddress, 0, sizeof(m_strAddress));
}

bool net_conn::init_peer_info()
{
    sockaddr_in sockaddr;
    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    int len = sizeof(sockaddr);
    if (SOCKET_ERROR != getpeername(get_socket(), (struct sockaddr*) & sockaddr, &len)) {
        set_peer_port(ntohs(sockaddr.sin_port));
        peer_addr_ = sockaddr.sin_addr.s_addr;
        return true;
    }

    return false;
}

char* net_conn::get_peer_addr() 
{
    if (m_strAddress[0] == '\0') {
        set_peer_addr(inet_ntoa(*(in_addr*) &peer_addr_));
    }
    return m_strAddress;
}

bool net_conn::write_recv_stream(const char* buff, size_t len) 
{
    recv_stream_.write((net_stream_type::value_type*) buff, len);
    return true;
}

bool net_conn::read_recv_stream(char* buff, size_t len)
{
    if (recv_stream_.size() >= len) 
    {
        recv_stream_.read((net_stream_type::value_type*) buff, len);
        return true;
    }

    return false;
}

size_t net_conn::get_recv_length()
{
    return recv_stream_.size();
}

void net_conn::upsate_last_active_tm()  
{
    last_active_tm_ = GetTickCount();
}

void net_conn::inc_post_read_count() 
{
    guard helper(mutex_);
    ++ post_read_count_;
}

void net_conn::dec_post_read_count()
{
    guard helper(mutex_);
    -- post_read_count_;
}

USHORT net_conn::get_post_read_count()
{
    guard helper(mutex_);
    return post_read_count_;
}

void net_conn::inc_post_write_count()
{
    guard helper(mutex_);
    ++ post_write_count_;
}

void net_conn::dec_post_write_count()
{
    guard helper(mutex_);
    -- post_write_count_;
}

USHORT net_conn::get_post_write_count()
{
    guard helper(mutex_);
    return post_write_count_;
}
