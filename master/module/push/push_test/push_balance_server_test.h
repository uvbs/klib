#pragma once


#include <core/singleton.h>

using namespace klib::core;

class push_balance_server_test : public singleton<push_balance_server_test>
{
public:
    push_balance_server_test(void);
    ~push_balance_server_test(void);

public:
    void start();

protected:


};

