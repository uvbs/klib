#pragma once

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
        INTERFACE_ENTRY(IID_PUSH_FRAMEWORK, push_client_i)
    END_INTERFACE_ENTRY

    //----------------------------------------------------------------------
    // push_client_i
    virtual bool set_option(std::string& domain, USHORT uport);

protected:
    udp_client client_;
};

