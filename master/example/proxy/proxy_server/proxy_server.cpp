// proxy_server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "proxy_server_t.h"

int _tmain(int argc, _TCHAR* argv[])
{

    proxy_server_t* p = proxy_server_t::instance();
    p->start();


	return 0;
}

