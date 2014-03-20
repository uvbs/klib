#include "stdafx.h"
#include "push_logic_server_test.h"


#include <aom/iframework_imp.h>


push_logic_server_test::push_logic_server_test(void)
{
}

push_logic_server_test::~push_logic_server_test(void)
{
}

void push_logic_server_test::start()
{
    klib::aom::iframework* pframework = klib::aom::framework::instance();

    HRESULT hr = pframework->load_module(_T("push_server.dll"));
    _ASSERT(S_OK == hr);

    push_logic_server_i* push_logic_server_i_;

    pframework->start();
    pframework->find_next_interface(IID_PUSH_LOGIC_SERVER, (void**)&push_logic_server_i_);

    //push_logic_server_i_->set_handle();
    push_logic_server_i_->start(9000);
}

