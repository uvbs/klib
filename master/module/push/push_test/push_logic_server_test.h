#pragma once


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



};

