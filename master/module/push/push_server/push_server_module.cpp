#include "stdafx.h"
#include "push_server_module.h"

winsock_init g_winsock_initor;

BEGIN_REGISTER_MODULES
    REGISTER_MODULE(push_balance_server_module)
    REGISTER_MODULE(push_logic_server_module)
END_REGISTER_MODULES

//----------------------------------------------------------------------
// 
push_balance_server_module::push_balance_server_module(void)
{
    work_port_ = 0;
    maintain_port_ = 0;
}

push_balance_server_module::~push_balance_server_module(void)
{
}

bool push_balance_server_module::start(USHORT work_port, USHORT maintain_port)
{
    this->work_port_ = work_port;
    this->maintain_port_ = maintain_port;

    return __super::start(work_port_);
}

void push_balance_server_module::on_query_logic_addr(ip_v4, USHORT, cmd_header*, net_archive*, BOOL&)
{

}

//----------------------------------------------------------------------
//
push_logic_server_module::push_logic_server_module(void)
{
}

push_logic_server_module::~push_logic_server_module(void)
{
}

void push_logic_server_module::on_query_curr_ver(ip_v4, USHORT, cmd_header*, net_archive*, BOOL&)
{

}