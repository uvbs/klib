#pragma once

#include "../../include/push_interface.h"
#include "../../../core/net/proto/dispatcher.h"

#include "../common/protocol.h"
#include <net/udp_server.h>
#include <net/proto/net_archive.h>
#include <net/proto/local_archive.h>
#include <core/lock_stl.h>
#include <kthread/auto_lock.h>

using namespace std::placeholders;

class push_balance_server_module : 
    public module_imp,
    public udp_server,
    public push_balance_server_i
{
public:
    push_balance_server_module(void);
    ~push_balance_server_module(void);

public:
    BEGIN_INTERFACE_ENTRY
        INTERFACE_ENTRY(IID_PUSH_BALANCE_SERVER, push_balance_server_i)
    END_INTERFACE_ENTRY

protected:
    BEGIN_REGISTER_DIAPATCH()
        REGISTER_DISPATCH(CMD_QUERY_LOGIC_SERVER, std::bind(&push_balance_server_module::on_query_logic_addr, this, _1, _2, _3, _4, _5))
    END_REGISTER_DISPATCH

protected:
    virtual bool add_logic_info(logic_addr_info info) ;
    virtual bool start(USHORT work_port, USHORT maintain_port);

protected:
    void on_query_logic_addr(ip_v4, USHORT, cmd_header*, net_archive*, BOOL&);

protected:
    USHORT work_port_;
    USHORT maintain_port_;
    volatile long query_logic_count_;

    logic_addr_info logic_addr_info_;
};


//----------------------------------------------------------------------
// Âß¼­·þÎñÆ÷
class push_logic_server_module : 
    public module_imp,
    public udp_server,
    public push_logic_server_i
{
public:
    push_logic_server_module(void);
    ~push_logic_server_module(void);

public:
    BEGIN_INTERFACE_ENTRY
        INTERFACE_ENTRY(IID_PUSH_BALANCE_SERVER, push_logic_server_i)
    END_INTERFACE_ENTRY

public:
    BEGIN_REGISTER_DIAPATCH()
        REGISTER_DISPATCH(CMD_QUERY_CURRENT_VERSION, std::bind(&push_logic_server_module::on_query_curr_ver, this, _1, _2, _3, _4, _5))
    END_REGISTER_DISPATCH

protected:
    void on_query_curr_ver(ip_v4, USHORT, cmd_header*, net_archive*, BOOL&);
};

