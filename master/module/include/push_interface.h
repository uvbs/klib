#ifndef _push_framework_interface_h_
#define _push_framework_interface_h_

// {5DBCA606-57CA-4d08-96A3-2A117550DB85}
static const GUID IID_PUSH_FRAMEWORK = 
{ 0x5dbca606, 0x57ca, 0x4d08, { 0x96, 0xa3, 0x2a, 0x11, 0x75, 0x50, 0xdb, 0x85 } };


#include <net/udp_client.h>

class push_client_i
{
    virtual bool set_option(std::string& domain, USHORT uport) = 0;
};

class push_server_i
{

};


#endif // _push_framework_interface_h_