#pragma once



#include "http_proxy_server_t.h"

class proxy_server_t : 
    public singleton<proxy_server_t>,
    public inet_tcp_handler
{
public:
    proxy_server_t(void);
    ~proxy_server_t(void);

    bool start() ;

protected:
    virtual bool on_connect(net_conn* pConn, bool bConnected = true) ;
    virtual bool on_disconnect(net_conn* pConn) ;
    virtual bool on_read(net_conn* pConn, const char* buff, size_t len) ;
    virtual bool on_write(net_conn* pConn, size_t len) ;
    virtual bool on_accept(net_conn* pListen, net_conn* pNewConn, bool bSuccess = true) ;

protected:
    proxy_server_i* http_proxy_;

};

