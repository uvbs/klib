#include "StdAfx.h"
#include "my_net_module.h"

#include "tcp_net_facade_imp.h"


my_net_module::my_net_module(void)
{
}

my_net_module::~my_net_module(void)
{
}

tcp_net_facade* my_net_module::create_tcp_facade()
{
    return new tcp_net_facade_imp;
}


BEGIN_REGISTER_MODULES
    REGISTER_MODULE(my_net_module)
END_REGISTER_MODULES