// mynet_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <crtdbg.h>

#include <comp/MyNet/imynetdef.h>


class MyHandler : public inet_event_handler
{
public:
    MyHandler(tcp_net_facade* client) {
        net_facade_ = client;
    }

    virtual bool on_accept(net_conn* pListen, net_conn* pNewConn, bool bSuccess) 
    {
        printf("接受连接，当前连接数 ：%d \r\n", net_facade_->get_net_conn_mgr()->get_conn_count());
        return true;
    }

    virtual bool on_disconnect(net_conn* pConn)  
    {
        printf("连接断开，当前连接数 ：%d \r\n", net_facade_->get_net_conn_mgr()->get_conn_count());
        // net_facade_->GetNetwork()->FreeConnection(pConn);
        return true;
    }

    virtual bool on_read(net_conn* pConn, const char* buff, size_t len)
    {
        printf(buff);
        return true;
    }
    virtual bool on_write(net_conn* pConn, size_t len) 
    {
        return true;
    }

    virtual bool on_connect(net_conn* pConn, bool bConnected /* = true */) 
    {
        printf("建立连接，当前连接数 ：%d \r\n", net_facade_->get_net_conn_mgr()->get_conn_count());

        std::string str = "GET / HTTP/1.1\r\n"
            "Host: www.baidu.com\r\n"
            "Accept: */*\r\n"
            "\r\n\r\n";

        net_facade_->get_network()->post_write(pConn, str.c_str(), str.size());
        
        return true;
    }

protected:
    tcp_net_facade* net_facade_;
};

int _tmain(int argc, _TCHAR* argv[])
{
    klib::aom::iframework* pframework = klib::aom::framework::instance();

    HRESULT hr = pframework->load_module(_T("mynet.dll"));
    _ASSERT(S_OK == hr);

    IMyNet* pNet = NULL;

    pframework->start();
    pframework->find_next_interface(IID_IMyNet, (void**)&pNet);

    tcp_net_facade* pClient = pNet->create_tcp_facade();

    MyHandler thehandler(pClient);
    pClient->init_client();
    pClient->add_event_handler(&thehandler);

    net_conn* pConn = pClient->get_network()->create_conn();
    pConn->set_peer_addr_st("www.baidu.com");
    pConn->set_peer_port(80);
    pClient->get_network()->post_connection(pConn);


    Sleep(-1);
	return 0;
}

