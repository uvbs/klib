#pragma once

#include "proxy_server_i.h"

#include <core/lock_stl.h>
#include <core/mem_buffer.h>
#include <pattern/fsm.h>

using namespace klib::stl;
using namespace klib::pattern;
using namespace klib::mem;
using namespace klib::pattern::fsm;

typedef mem_buffer<1024*1024, 1024, FALSE> buffer_list_type;


enum http_proxy_status
{
    status_local_read,
    status_connect_remote,
    status_interact,
};

enum http_proxy_event
{
    event_connect,
    event_disconnect,
    event_read,
    event_send,
};

class http_proxy_server_t;
class http_prxy_context;
class proxy_event_base : public FsmEvent
{
public:
    net_conn* local_conn_;
    http_prxy_context* ctx_;
    http_proxy_server_t* svr_;
};

class proxy_event_connect : public proxy_event_base
{
public:
    net_conn* pconn;
    bool bConnected;
};

class proxy_event_read : public proxy_event_base
{
public:
    net_conn* pconn;
    const char* buff;
    size_t len;
};

class proxy_event_disconnect : public proxy_event_base
{
public:
    net_conn* pconn;
};

BEGIN_STATE_DECLARE(local_read_state, status_local_read)

virtual void OnEvent(FsmEvent* e, UINT& uNewStateID);
END

BEGIN_STATE_DECLARE(connect_remote_state, status_connect_remote)

virtual void OnEvent(FsmEvent* e, UINT& uNewStateID);
END

BEGIN_STATE_DECLARE(interactive_state, status_interact)

virtual void OnEvent(FsmEvent* e, UINT& uNewStateID) ;
END


///< 状态机的定义
BEGIN_FSM(http_proxy_fsm)
    BEGIN_REGISTER_STATE()
        REGISTER_INIT_STATE(local_read_state)
        REGISTER_STATE(connect_remote_state)
        REGISTER_STATE(interactive_state)
    END_REGISTER_STATE

public:

END


class http_prxy_context : public proxy_context
{
public:
    buffer_list_type recv_buff_;
    http_proxy_fsm   fsm_;
};

class http_proxy_server_t : public proxy_server_imp
{
public:
    http_proxy_server_t(void);
    ~http_proxy_server_t(void);

protected:
    virtual void on_accept(net_conn* listen_conn, net_conn* accept_conn, bool bsuccess);
    virtual void on_connect(net_conn* pconn, bool bConnected);
    virtual void on_read(net_conn* pconn, const char* buff, size_t len);
    virtual void on_disconnect(net_conn* pconn);

public:
    lock_bucket_map<net_conn*, http_prxy_context*, 16>      conn_session_map_;
    CObjectPool<http_prxy_context, 10000, 10000>            proxy_ctx_pool_;
    buffer_list_type                                        recv_buff_;
};

