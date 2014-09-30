#include "stdafx.h"
#include "http_proxy_server_t.h"

#include <util/strutil.h>
#include <core/scope_guard.h>

bool extract_info(const std::string& str, std::string& host, USHORT& uport)
{
    if (strncmp("CONNECT ", str.c_str(), 7) == 0) 
    {
        size_t space_pos = str.find(" ", 8);
        if (space_pos != std::string::npos) 
        {
            std::string str_tmp_host = str.substr(8, space_pos - 8);
            if (str.compare("http://") == 0) 
            {
                str_tmp_host = str_tmp_host.substr(7);
            }
            str_tmp_host = klib::util::trim(str_tmp_host);

            size_t quto_pos = str_tmp_host.find(":");
            if (quto_pos == std::string::npos) 
            {
                uport = 80;
                host = str_tmp_host;
            }
            else 
            {
                std::string str_port = str_tmp_host.substr(quto_pos + 1);
                char* tmp;
                uport = (USHORT) strtoul(str_port.c_str(), &tmp, 10);

                host = str_tmp_host.substr(0, quto_pos);
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        size_t host_begin_pos = str.find("Host:");
        size_t host_end_pos   = str.find("\r\n", host_begin_pos);
        if (host_begin_pos != std::string::npos && host_end_pos != std::string::npos) 
        {
            std::string str_tmp_host = str.substr(host_begin_pos + 5, host_end_pos - host_begin_pos - 5);
            str_tmp_host = klib::util::trim(str_tmp_host);

            size_t quto_pos = str_tmp_host.find(":");
            if (quto_pos == std::string::npos) 
            {
                uport = 80;
                host = str_tmp_host;
            }
            else 
            {
                std::string str_port = str_tmp_host.substr(quto_pos + 1);
                char* tmp;
                uport = (USHORT) strtoul(str_port.c_str(), &tmp, 10);

                host = str_tmp_host.substr(0, quto_pos);
            }
            
            return true;
        }
        return false;
    }
    return true;
}

bool extract_content(const std::string& str, std::string& content)
{
    // 提取 GET或者POST后面的内容
    std::string query_str = klib::util::getMid<std::string>(str, " ", " ");
    query_str = klib::util::trim(query_str);

    size_t query_pos = query_str.find("://");
    if (query_pos != std::string::npos) 
    {
        query_pos += 3;
        while (query_pos < query_str.size())
        {
            if (query_str[query_pos] == '/') {
                break;
            }
            ++ query_pos;
        }

        std::string str_q = query_str.substr(query_pos);
        content = klib::util::Replace(str, query_str, str_q);
    }
    else
    {
        content = str;
    }

    // 去除Proxy参数
    content = klib::util::replaceMid(content, "Proxy-", "\r\n", "");

    return true;
}

void local_read_state_on_event(FsmEvent* e)
{
    //KLIB_ASSERT(e->get_evt_type() == event_read ||
    //            e->get_evt_type() == event_disconnect);

    if (e->get_evt_type() == event_disconnect) 
    {
        //do nothing
    }
    else if (e->get_evt_type() == event_read) 
    {
        proxy_event_read* evt = (proxy_event_read*)e;
        evt->ctx_->recv_buff_.append(evt->buff, evt->len);

        // 解析,如果成功的话，则发送数据
        int index = evt->ctx_->recv_buff_.find("\r\n\r\n", 4, 0, false);
        if (-1 == index) {
            // 继续收头部数据
            return;
        }

        std::string str_host;
        USHORT uport;
        std::string str_req;
        str_req.resize(evt->ctx_->recv_buff_.size());
        evt->ctx_->recv_buff_.copy(
            (char*)str_req.c_str(), 
            0, 
            evt->ctx_->recv_buff_.size());

        if (extract_info(str_req, str_host, uport)) 
        {
            net_conn_ptr new_conn = evt->svr_->get_network()->try_connect(
                str_host.c_str(), 
                uport, 
                evt->ctx_);
            if (nullptr != new_conn) 
            {
                http_prxy_context* ctx = (http_prxy_context*)evt->pconn->get_bind_key();
                if (nullptr == ctx) {
                    return;
                }
                ctx->remote_conn_ = new_conn;
            }
            else
            {
                printf("try connect: %s:%d failed ~~~~~~ \r\n", str_host.c_str(), uport);
                evt->ctx_->local_conn_->dis_connect();
            }
        }
        else
        {
            printf("extract info failed :%.*s ~~~~~~ \r\n", evt->len, evt->buff);
            if (evt ->ctx_->local_conn_) {
                evt->ctx_->local_conn_->dis_connect();
            }
        }
    }
    else if (e->get_evt_type() == event_connect) {
        proxy_event_connect* evt = (proxy_event_connect*)e;
        if (!evt->bConnected && evt->ctx_->local_conn_) 
        {
            evt->ctx_->local_conn_->dis_connect();
            printf("connect %s:%d failed ~~~~~~ \r\n", 
                evt->pconn->get_peer_addr_str(), 
                evt->pconn->get_peer_port());
            return;
        }
        evt->ctx_->remote_conn_ = evt->pconn;

        std::string str_req;
        str_req.resize(evt->ctx_->recv_buff_.size());
        evt->ctx_->recv_buff_.copy((char*)str_req.c_str(), 
            0, 
            evt->ctx_->recv_buff_.size());

        std::string content;
        if (extract_content(str_req, content)) 
        {
            evt->svr_->get_network()->try_write(evt->pconn, 
                content.c_str(), 
                content.size());
        }
    }
}

void connect_remote_state_on_event(FsmEvent* e)
{
    //KLIB_ASSERT(e->get_evt_type() == event_connect||
    //            e->get_evt_type() == event_disconnect);

    if (e->get_evt_type() == event_connect) 
    {
        proxy_event_connect* evt = (proxy_event_connect*)e;
        if (!evt->bConnected) 
        {
            evt->ctx_->local_conn_->dis_connect();
            printf("connect %s:%d failed ~~~~~~ \r\n", 
                evt->pconn->get_peer_addr_str(), 
                evt->pconn->get_peer_port());
            return;
        }
        evt->ctx_->remote_conn_ = evt->pconn;

        std::string str_req;
        str_req.resize(evt->ctx_->recv_buff_.size());
        evt->ctx_->recv_buff_.copy((char*)str_req.c_str(), 
            0, 
            evt->ctx_->recv_buff_.size());

        std::string content;
        if (extract_content(str_req, content)) 
        {
            evt->svr_->get_network()->try_write(evt->pconn, 
                content.c_str(), 
                content.size());
        }
    }
}

void interactive_state_on_event(FsmEvent* e)
{
    //KLIB_ASSERT(e->get_evt_type() == event_read ||
    //    e->get_evt_type() == event_disconnect);

    if (e->get_evt_type() == event_read) 
    {
        proxy_event_read* evt = (proxy_event_read*) e;
        
        if (evt->pconn == evt->ctx_->local_conn_) 
        {
            evt->svr_->get_network()->try_write(evt->ctx_->remote_conn_, 
                evt->buff, 
                evt->len);
        }
        else if (nullptr != evt->ctx_->local_conn_)
        {
            evt->svr_->get_network()->try_write(evt->ctx_->local_conn_, 
                evt->buff, 
                evt->len);
        }
    }
}

//----------------------------------------------------------------------
//
http_proxy_server_t::http_proxy_server_t(void):
    connected_num_(0)
{
}

http_proxy_server_t::~http_proxy_server_t(void)
{
}

void http_proxy_server_t::on_accept(net_conn_ptr listen_conn, net_conn_ptr accept_conn, bool bsuccess /*= true*/)
{
    http_prxy_context* ctx = NULL;

    if (!bsuccess) {
        return;
    }

    ctx = proxy_ctx_pool_.Alloc();
    ctx->local_conn_ = accept_conn;
    ctx->local_conn_->set_bind_key(ctx);
    ctx->recv_buff_.clear();

    ++ connected_num_;
}

void http_proxy_server_t::on_connect(net_conn_ptr pconn, bool bConnected)
{
    http_prxy_context* ctx = (http_prxy_context*) pconn->get_bind_key();
    if (nullptr == ctx) {
        pconn->dis_connect();
        return;
    }

    proxy_event_connect evt;
    evt.set_evt_type(event_connect) ;
    evt.pconn       = pconn;
    evt.bConnected  = bConnected;

    evt.ctx_  = ctx;
    evt.svr_  = this;
    local_read_state_on_event(&evt);

    ++ connected_num_;
    return;
}

void http_proxy_server_t::on_read(net_conn_ptr pconn, const char* buff, size_t len)
{
    http_prxy_context* ctx = (http_prxy_context*) pconn->get_bind_key();
    if (nullptr == ctx) {
        pconn->dis_connect();
        return;
    }

    proxy_event_read evt;
    evt.set_evt_type(event_read);
    evt.pconn = pconn;
    evt.buff  = buff;
    evt.len   = len;

    evt.ctx_  = ctx;
    evt.svr_  = this;
    if (nullptr == ctx->remote_conn_) {
        local_read_state_on_event(&evt);
    }
    else {
        interactive_state_on_event(&evt);
    }
}

void http_proxy_server_t::on_disconnect(net_conn_ptr pconn)
{
    http_prxy_context* ctx = (http_prxy_context*) pconn->get_bind_key();
    if (nullptr == ctx) {
        return;
    }

    proxy_event_disconnect evt;
    evt.set_evt_type(event_disconnect);
    evt.pconn = pconn;
    evt.ctx_  = ctx;
    evt.svr_  = this;

    //----------------------------------------------------------------------
    if (ctx->local_conn_ == pconn) 
    {
        printf("local connection disconnect ~~~~~~ \r\n");
        if (ctx->remote_conn_) {
            ctx->remote_conn_->dis_connect();
        }
    }
    else
    {
        printf("remote connection disconnect ~~~~~~ \r\n");
        if (ctx->local_conn_) {
            ctx->local_conn_->dis_connect();
        }
    }

    if (nullptr != ctx->local_conn_) {
        ctx->local_conn_->set_bind_key(nullptr);
        proxy_ctx_pool_.Free(ctx);

        printf("ctx freed connected num: %llu \r\n", connected_num_);
        -- connected_num_;
    }
}
