#pragma once

class proxy_context
{
public:
    proxy_context() : 
        local_conn_(NULL), 
        remote_conn_(NULL),
        verified_(false)
    {
    }

    net_conn* local_conn_;
    net_conn* remote_conn_;
    bool  verified_;
};

class proxy_server_i
{
public:
    proxy_server_i(void);
    virtual ~proxy_server_i(void);


public:
    virtual network_i* get_network() = 0;
    virtual void on_accept(net_conn* pconn, net_conn* paccept_conn, bool bsuccess) = 0;
    virtual void on_connect(net_conn* pconn, bool bConnected) = 0;
    virtual void on_read(net_conn* pconn, const char* buff, size_t len) = 0;
    virtual void on_disconnect(net_conn* pconn) = 0;
};

class proxy_server_imp : public proxy_server_i
{
public:
    network_i* get_network() 
    {
        return network_i_;
    }

public:
    network_i* network_i_;
};
