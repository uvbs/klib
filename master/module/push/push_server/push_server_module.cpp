#include "stdafx.h"
#include "push_server_module.h"

#include "../common/protocol_st.h"

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
    query_logic_count_ = 0;
}

push_balance_server_module::~push_balance_server_module(void)
{
}

bool push_balance_server_module::add_logic_info(logic_addr_info info) 
{
    logic_addr_info_ = info;
    return true;
}

bool push_balance_server_module::start(USHORT work_port, USHORT maintain_port)
{
    this->work_port_ = work_port;
    this->maintain_port_ = maintain_port;

    setlocale(LC_ALL, "chs");

    return __super::start(work_port_);
}

void push_balance_server_module::on_query_logic_addr(ip_v4 remote_addr, 
    USHORT remote_port, cmd_header* header, net_archive*ar, BOOL&)
{
    PT_CMD_QUERY_LOGIC_SERVER ptQuery;
    PT_CMD_QUERY_LOGIC_SERVER_ACK ack;

    local_archive<> localar;

    InterlockedIncrement(&query_logic_count_);

    (*ar) >> ptQuery;

    ack.uLogicIP       =  logic_addr_info_.addr_;
    ack.uLogicUdpPort  =  logic_addr_info_.uport_;
    ack.uLogicTcpPort  =  0;

    cmd_header header_;
    header_.cmd = CMD_QUERY_LOGIC_SERVER_ACK;

    localar << header_;
    localar << ack;

    socket_.send_to(remote_addr, remote_port, localar.get_buff(), localar.get_data_len());
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