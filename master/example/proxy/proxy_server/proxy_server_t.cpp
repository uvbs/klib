#include "StdAfx.h"
#include "proxy_server_t.h"

#include <aom/iframework_imp.h>
#include <comp/MyNet/imynetdef.h>

using namespace klib::aom;

proxy_server_t::proxy_server_t(void)
{
}

proxy_server_t::~proxy_server_t(void)
{
}

bool proxy_server_t::start()
{
    klib::aom::iframework* pframework = klib::aom::framework::instance();

    HRESULT hr = pframework->load_module(_T("mynet.dll"));
    _ASSERT(S_OK == hr);

    imy_net* pNet = NULL;
    pframework->start();
    pframework->find_next_interface(IID_imy_net, (void**)&pNet);
    if (NULL == pNet) {
        return false;
    }

    http_proxy_ = new http_proxy_server_t;
    if (NULL == http_proxy_) {
        return false;
    }

    tcp_net_facade* tcp_facade_ = pNet->create_tcp_facade();
    tcp_facade_->init();
    tcp_facade_->add_event_handler(this);
    tcp_facade_->get_network()->try_listen(8080, http_proxy_);
    ((proxy_server_imp*)http_proxy_)->network_i_ = tcp_facade_->get_network();


    return true;
}

bool proxy_server_t::on_connect(net_conn_ptr pConn, bool bConnected /*= true*/) 
{
    http_proxy_->on_connect(pConn, bConnected);
    return true;
}

bool proxy_server_t::on_disconnect(net_conn_ptr pConn) 
{
    http_proxy_->on_disconnect(pConn);
    return true;
}

bool proxy_server_t::on_read(net_conn_ptr pConn, const char* buff, size_t len) 
{
    http_proxy_->on_read(pConn, buff, len);
    return true;
}

bool proxy_server_t::on_write(net_conn_ptr pConn, size_t len) 
{

    return true;
}

bool proxy_server_t::on_accept(net_conn_ptr pListen, net_conn_ptr pNewConn, bool bSuccess /* = true*/) 
{
    http_proxy_->on_accept(pListen, pNewConn, bSuccess);
    return true;
}
