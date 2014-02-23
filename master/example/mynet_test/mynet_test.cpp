// mynet_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <crtdbg.h>

#include <comp/MyNet/imynetdef.h>


class my_tcp_handler : public inet_tcp_handler
{
public:
    my_tcp_handler(tcp_net_facade* client) {
        net_facade_ = client;
    }

    virtual bool on_accept(net_conn* pListen, net_conn* pNewConn, bool bSuccess) 
    {
        printf("接受连接，当前连接数 ：%d \r\n", net_facade_->get_net_conn_mgr()->get_conn_count());

        if (pListen->get_bind_key() == 0) 
        {
            std::string str = 
                "HTTP/1.1 200 OK\r\n"
                "Server: Microsoft-IIS/4.0\r\n"
                "Date: Mon, 3 Jan 2005 13:13:33 GMT\r\n"
                "Content-Type: text/html\r\n"
                "Last-Modified: Mon, 11 Jan 2004 13:23:42 GMT\r\n"
                "Content-Length: 12\r\n"
                "\r\n"
                "hello world!"
                ;

            pListen->set_bind_key(pListen);

            net_facade_->get_network()->try_write(pNewConn, str.c_str(), str.size());
        }
        return true;
    }

    virtual bool on_disconnect(net_conn* pConn)  
    {
        printf("连接断开，当前连接数 ：%d \r\n", net_facade_->get_net_conn_mgr()->get_conn_count());
    

        return true;
    }

    virtual bool on_read(net_conn* pConn, const char* buff, size_t len)
    {
        printf("%.*s", len, buff);
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

        net_facade_->get_network()->try_write(pConn, str.c_str(), str.size());
    
        
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

    imy_net* pNet = NULL;

    pframework->start();
    pframework->find_next_interface(IID_imy_net, (void**)&pNet);

    tcp_net_facade* tcp_facade_ = pNet->create_tcp_facade();

    my_tcp_handler thehandler(tcp_facade_);
    tcp_facade_->init();
    tcp_facade_->add_event_handler(&thehandler);

    net_conn* pConn = NULL;
    for (int i=0; i<100; ++i)
    {
        pConn = tcp_facade_->get_network()->try_connect("www.baidu.com", 80);
    }

    tcp_facade_->get_network()->try_listen(900);
    for (int i=0; i<100; ++i)
    {
        tcp_facade_->get_network()->try_connect("127.0.0.1", 900);
    }

    while (true)
    {
        printf("connection num: %d \r\n", tcp_facade_->get_net_conn_mgr()->get_conn_count());

        Sleep(2000);
    }
	return 0;
}

