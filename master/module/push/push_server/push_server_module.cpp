#include "stdafx.h"
#include "push_server_module.h"

#include "../common/protocol_st.h"

winsock_init g_winsock_initor;

using namespace logic;
using namespace balance;

BEGIN_REGISTER_MODULES
    REGISTER_MODULE(push_balance_server_module)
    REGISTER_MODULE(push_logic_server_module)
END_REGISTER_MODULES

