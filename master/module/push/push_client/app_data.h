#pragma once

#include <macro.h>
#include "../../include/push_interface.h"


#define  CURRENT_CLIENT_VERSION_STRING "1.1.1.0"
#define  CURRENT_CLIENT_VERSION_VALUE  inet_addr(CURRENT_CLIENT_VERSION_STRING)

class app_data : public singleton<app_data>
{
    DECLARE_SINGLETON_CLASS(app_data)

public:
    DEFINE_ACCESS_FUN_REF2(std::string, channel, channel_)
    DEFINE_ACCESS_FUN_REF2(std::string, mac, mac_)
    DEFINE_ACCESS_FUN_REF2(std::string, user, user_)
    DEFINE_ACCESS_FUN_REF2(std::string, pass, pass_)
    DEFINE_ACCESS_FUN_REF2(std::string, appid, appid_)
    DEFINE_ACCESS_FUN_REF2(UINT64,      last_msg_id, last_msg_id_)

    DEFINE_ACCESS_FUN_REF2(std::string, svr_domain, svr_domain_);
    DEFINE_ACCESS_FUN_REF2(USHORT,      svr_port,   svr_port_);
    DEFINE_ACCESS_FUN_REF2(push_client_callback, client_callback, client_callback_);

    DEFINE_ACCESS_FUN_REF2(UINT32,      logic_addr,   logic_addr_);
    DEFINE_ACCESS_FUN_REF2(USHORT,      logic_port,   logic_port_);

    void save(){}
protected:
    std::string channel_;
    std::string mac_;
    std::string user_;
    std::string pass_;
    std::string appid_;
    UINT64 last_msg_id_;

    std::string svr_domain_;
    USHORT      svr_port_;

    UINT32      logic_addr_;
    USHORT      logic_port_;

    push_client_callback client_callback_;
};

