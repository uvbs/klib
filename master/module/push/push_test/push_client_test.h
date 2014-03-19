#pragma once

#include <core/singleton.h>

using namespace klib::core;


class push_client_test : public singleton<push_client_test>
{
public:
    push_client_test(void);
    ~push_client_test(void);

    void start();

public:
    void handle_msg(push_msg_ptr msg);

protected:


};

