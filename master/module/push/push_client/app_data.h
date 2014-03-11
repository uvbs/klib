#pragma once

#include <macro.h>

class app_data : public singleton<app_data>
{
    DECLARE_SINGLETON_CLASS(app_data)
public:

    DEFINE_ACCESS_FUN_REF2(std::string, domain, svr_domain_);
    DEFINE_ACCESS_FUN_REF2(USHORT,      port,   svr_port_);

protected:
    std::string svr_domain_;
    USHORT      svr_port_;
};

