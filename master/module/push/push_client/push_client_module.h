#pragma once


#include "../../include/push_interface.h"

//----------------------------------------------------------------------
// 推送模块客户端定义
class push_client_module : 
    public module_imp,
    public push_client_i
{
public:
    push_client_module(void);
    ~push_client_module(void);

public:
    BEGIN_INTERFACE_ENTRY
        INTERFACE_ENTRY(IID_PUSH_CLIENT, push_client_i)
    END_INTERFACE_ENTRY

    //----------------------------------------------------------------------
    // push_client_i
    virtual bool set_option(std::string& domain, USHORT uport);
    virtual bool set_data_callback(handle_data_callback& callback_);
    virtual bool set_msg_callback(handle_msg_callback& callback);
    virtual bool set_client_info(const std::string& channel,
        const std::string& mac,
        const std::string& user,
        const std::string& pass,
        const std::string& last_msg_id, 
        const std::string& appid) ;
    virtual bool start();
    virtual bool stop();
    virtual push_client_status get_status();

protected:
};

