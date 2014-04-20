#pragma once


#include <core/timer_mgr.h>
#include <core/singleton.h>
using namespace klib::core;


class push_upgrade_test : public singleton<push_upgrade_test>
{
public:
    push_upgrade_test(void);
    ~push_upgrade_test(void);

public:
    void test_upgrade();
};

