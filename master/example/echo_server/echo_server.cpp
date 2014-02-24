// echo_server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include <inttype.h>
#include <aom/iframework_imp.h>
#include <comp/MyNet/imynetdef.h>


class my_tcp_handler : public inet_tcp_handler
{
public:
    my_tcp_handler(tcp_net_facade* client) {
        net_facade_ = client;
    }

    virtual bool on_accept(net_conn* pListen, net_conn* pNewConn, bool bSuccess) 
    {
        //printf("接受连接，当前连接数 ：%d \r\n", net_facade_->get_net_conn_mgr()->get_conn_count());
        return true;
    }

    virtual bool on_disconnect(net_conn* pConn)  
    {
        //printf("连接断开，当前连接数 ：%d \r\n", net_facade_->get_net_conn_mgr()->get_conn_count());
        return true;
    }

    virtual bool on_read(net_conn* pConn, const char* buff, size_t len)
    {
        printf("%.*s", len, buff);
        net_facade_->get_network()->try_write(pConn, buff, len);

        return true;
    }

    virtual bool on_write(net_conn* pConn, size_t len) 
    {
        return true;
    }

    virtual bool on_connect(net_conn* pConn, bool bConnected /* = true */) 
    {
        return true;
    }

protected:
    tcp_net_facade* net_facade_;
};


int _tmain(int argc, _TCHAR* argv[])
{
    klib::aom::iframework* pframework = klib::aom::framework::instance();

    HRESULT hr = pframework->load_module(_T("mynet.dll"));
    KLIB_ASSERT(S_OK == hr);

    imy_net* pNet = NULL;

    pframework->start();
    pframework->find_next_interface(IID_imy_net, (void**)&pNet);

    tcp_net_facade* tcp_facade_ = pNet->create_tcp_facade();

    my_tcp_handler thehandler(tcp_facade_);
    tcp_facade_->init();
    tcp_facade_->add_event_handler(&thehandler);


    tcp_facade_->get_network()->try_listen(9000);

    
    printf("echo server started ... \r\n");
    Sleep(-1);

	return 0;
}

