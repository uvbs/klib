#include "StdAfx.h"
#include "net_conn.h"

net_conn::net_conn(void)
{
    post_read_count_ = 0;
    post_write_count_ = 0;
    is_closing_ = FALSE;
    last_active_tm_ = 0;
    peer_port_ = 0;
    local_port_ = 0;
    bind_key_ = 0;
    readed_bytes_ = 0;
    writed_bytes_ = 0;
    memset(peer_addr_str_, 0, sizeof(peer_addr_str_));
}

net_conn::~net_conn(void)
{
    post_read_count_ = 0;
    post_write_count_ = 0;
    is_closing_ = FALSE;
    last_active_tm_ = 0;
    peer_port_ = 0;
    local_port_ = 0;
    bind_key_ = 0;
    readed_bytes_ = 0;
    writed_bytes_ = 0;
    memset(peer_addr_str_, 0, sizeof(peer_addr_str_));
}

bool net_conn::init_peer_info()
{
    sockaddr_in sockaddr;
    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    int len = sizeof(sockaddr);
    if (SOCKET_ERROR != getpeername(get_socket(), (struct sockaddr*) & sockaddr, &len)) 
    {
        set_peer_port(ntohs(sockaddr.sin_port));
        peer_addr_dw_ = sockaddr.sin_addr.s_addr;
        return true;
    }

    return false;
}

// void net_conn::set_peer_addr_str(const char* straddr)
// {
//     strncpy(peer_addr_str_, straddr, sizeof(peer_addr_str_) - 1);
// }

char* net_conn::get_peer_addr_str() 
{
    if (peer_addr_str_[0] == '\0') {
        set_peer_addr_str(inet_ntoa(*(in_addr*) &peer_addr_str_));
    }
    return peer_addr_str_;
}

void net_conn::dis_connect() 
{  
    closesocket(m_socket); m_socket = INVALID_SOCKET; 
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

bool net_conn::write_send_stream(const char* buff, size_t len) 
{
    send_stream_.write((unsigned char*)buff, len);
    return true;
}

bool net_conn::mark_send_stream(size_t len)
{
    return send_stream_.skip_read(len);
}

bool net_conn::read_send_stream(char* buff, size_t len)
{
    send_stream_.read((unsigned char*)buff, len);
    return true;
}

size_t net_conn::get_send_length()
{
    return send_stream_.size();
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
