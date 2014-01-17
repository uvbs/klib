#include "StdAfx.h"
#include "MyNetModule.h"

#include "tcpclient_imp.h"


CMyNetModule::CMyNetModule(void)
{
}

CMyNetModule::~CMyNetModule(void)
{
}

tcpclient* CMyNetModule::CreateTcpClient()
{
    return new tcpclient_imp;
}


BEGIN_REGISTER_MODULES
    REGISTER_MODULE(CMyNetModule)
END_REGISTER_MODULES