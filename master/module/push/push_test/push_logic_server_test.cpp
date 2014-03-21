#include "stdafx.h"
#include "push_logic_server_test.h"


#include <aom/iframework_imp.h>


push_logic_server_test::push_logic_server_test(void)
{
    push_logic_server_i_ = nullptr;
    msg_id_ = 1;
}

push_logic_server_test::~push_logic_server_test(void)
{
}

void push_logic_server_test::start()
{
    klib::aom::iframework* pframework = klib::aom::framework::instance();

    HRESULT hr = pframework->load_module(_T("push_server.dll"));
    _ASSERT(S_OK == hr);


    pframework->start();
    pframework->find_next_interface(IID_PUSH_LOGIC_SERVER, (void**)&push_logic_server_i_);

    //push_logic_server_i_->set_handle();
    push_logic_server_i_->start(9000);

    // add send msg timer 
    timer_func_type f = std::bind(&push_logic_server_test::timer_send_msg, this);
    tmr_mgr_.add(20*1000, f);
    tmr_mgr_.start();
}

bool push_logic_server_test::timer_send_msg()
{
    push_msg_ptr msg(new push_msg);
    msg->set_msg_id(msg_id_ ++ );
    msg->set_msg_type(1);
    msg->set_content("hell world from logic server ~~~~ ");
    msg->set_show_time(444);
    push_logic_server_i_->broad_cast_msg(msg);

    return true;
}
