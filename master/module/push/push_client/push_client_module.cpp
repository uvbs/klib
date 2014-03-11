#include "stdafx.h"
#include "push_client_module.h"


winsock_init g_winsock_initor;

BEGIN_REGISTER_MODULES
    REGISTER_MODULE(push_client_module)
END_REGISTER_MODULES

push_client_module::push_client_module(void)
{
    
}

push_client_module::~push_client_module(void)
{
}
