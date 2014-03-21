#pragma once


#include <core/timer_mgr.h>
#include <core/singleton.h>
using namespace klib::core;


class push_logic_server_test : public singleton<push_logic_server_test>
{
public:
    push_logic_server_test(void);
    ~push_logic_server_test(void);

public:
    void start();

protected:
    bool timer_send_msg();

protected:
    size_t msg_id_;
    push_logic_server_i* push_logic_server_i_;
    timer_mgr tmr_mgr_;

};

