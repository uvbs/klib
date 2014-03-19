#include "stdafx.h"
#include "push_balance_server.h"

#include "../common/protocol_st.h"


namespace balance
{


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
    MyPrtLog("balance server starting...");

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
    ack.uLogicUdpPort  =  KHTON16(logic_addr_info_.port_);
    ack.uLogicTcpPort  =  0;

    cmd_header header_ack(CMD_QUERY_LOGIC_SERVER_ACK);
    localar << header_ack;
    localar << ack;

    socket_.send_to(remote_addr, remote_port, localar.get_buff(), localar.get_data_len());
}


}