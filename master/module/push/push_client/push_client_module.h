#pragma once


class push_client_module : public module_imp
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


protected:
    udp_client client_;
};

