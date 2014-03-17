#pragma once

#include <core/singleton.h>
#include <comp/sign_verify/sign_helper.h>

namespace logic
{



class server_data : public singleton<server_data>
{
public:
    ~server_data(void);

public:
    void init();
    sign_helper& get_sign_helper(); 

protected:
    sign_helper                         sign_helper_;
};


}
