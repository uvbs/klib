#include "stdafx.h"
#include "push_client_module.h"

#include "app_data.h"

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

bool push_client_module::set_option(std::string& domain, USHORT uport)
{
    app_data::instance()->set_domain(domain);
    app_data::instance()->set_port(uport);
    return true;
}
