#include "stdafx.h"
#include "push_balance_server_test.h"

#include <aom/iframework_imp.h>

push_balance_server_test::push_balance_server_test(void)
{
}

push_balance_server_test::~push_balance_server_test(void)
{
}

void push_balance_server_test::start()
{
    klib::aom::iframework* pframework = klib::aom::framework::instance();

    HRESULT hr = pframework->load_module(_T("push_server.dll"));
    _ASSERT(S_OK == hr);

    push_balance_server_i* push_balance_server_i_;

    pframework->start();
    pframework->find_next_interface(IID_PUSH_BALANCE_SERVER, (void**)&push_balance_server_i_);

    logic_addr_info info;
    info.addr_ = inet_addr("127.0.0.1");
    info.port_ = 9000;
    push_balance_server_i_->add_logic_info(info);
    push_balance_server_i_->start(8000, 9000);

}