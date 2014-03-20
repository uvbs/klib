// push_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <string>
#include <net/winsock_init.h>
#include <net/proto/tlv_archive.h>

#include "push_client_test.h"
#include "push_balance_server_test.h"
#include "push_logic_server_test.h"

klib::net::winsock_init g_sock_initor;
#pragma comment(lib, "ws2_32")

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc < 2) {
        return -1;
    }

    if (_tcsicmp(argv[1], _T("client")) == 0) 
    {
        push_client_test::instance()->start();
    }
    else if (_tcsicmp(argv[1], _T("balance_server")) == 0) 
    {
        push_balance_server_test::instance()->start();
    }
    else if (_tcsicmp(argv[1], _T("logic_server")) == 0) 
    {
        push_logic_server_test::instance()->start();
    }

    Sleep(-1);

	return 0;
}

