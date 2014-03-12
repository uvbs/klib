#include "stdafx.h"
#include "push_client_module.h"

#include "app_data.h"
#include "push_client_imp.h"

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
    app_data::instance()->set_svr_domain(domain);
    app_data::instance()->set_svr_port(uport);
    return true;
}

bool push_client_module::set_data_callback(handle_data_callback& callback_)
{
    app_data::instance()->set_data_callback(callback_);
    return true;
}

bool push_client_module::set_msg_callback(handle_msg_callback& callback)
{
    app_data::instance()->set_msg_callback(callback);
    return true;
}

bool push_client_module::set_client_info(const std::string& channel,
    const std::string& mac,
    const std::string& user,
    const std::string& pass,
    const std::string& last_msg_id, 
    const std::string& appid) 
{
    app_data* data_ = app_data::instance();

    return true;
}

bool push_client_module::start()
{
    push_client_imp::instance()->start();
    return true;
}

bool push_client_module::stop()
{
    push_client_imp::instance()->stop();
    return true;
}

push_client_status push_client_module::get_status()
{
    return push_client_imp::instance()->get_status();
}