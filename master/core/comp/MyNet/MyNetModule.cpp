#include "StdAfx.h"
#include "MyNetModule.h"

#include "tcp_net_facade_imp.h"


CMyNetModule::CMyNetModule(void)
{
}

CMyNetModule::~CMyNetModule(void)
{
}

tcp_net_facade* CMyNetModule::CreateTcpClient()
{
    return new tcp_net_facade_imp;
}


BEGIN_REGISTER_MODULES
    REGISTER_MODULE(CMyNetModule)
END_REGISTER_MODULES