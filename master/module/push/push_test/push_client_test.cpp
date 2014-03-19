#include "stdafx.h"
#include "push_client_test.h"

#include <aom/iframework_imp.h>
using namespace klib::aom;

push_client_test::push_client_test(void)
{
}

push_client_test::~push_client_test(void)
{
}

void push_client_test::start()
{
    klib::aom::iframework* pframework = klib::aom::framework::instance();

    HRESULT hr = pframework->load_module(_T("push_client.dll"));
    _ASSERT(S_OK == hr);

    push_client_i* push_client_i_;

    pframework->start();
    pframework->find_next_interface(IID_PUSH_CLIENT, (void**)&push_client_i_);

    push_client_i_->set_client_info("channel",
        "22-22-22-22-22-22",
        "client",
        "pass",
        "1",
        "1111");

    handle_msg_callback f = std::bind(&push_client_test::handle_msg, this, std::placeholders::_1);
    push_client_i_->set_msg_callback(f);
    push_client_i_->set_logic_server_info("www.baidu.com", 8000);
}

void push_client_test::handle_msg(push_msg_ptr msg)
{

}