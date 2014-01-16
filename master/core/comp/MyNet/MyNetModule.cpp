#include "StdAfx.h"
#include "MyNetModule.h"

#include "INetClientImp.h"


CMyNetModule::CMyNetModule(void)
{
}

CMyNetModule::~CMyNetModule(void)
{
}

INetTcpClient* CMyNetModule::CreateTcpClient()
{
    return new INetTcpClientImp;
}


BEGIN_REGISTER_MODULES
    REGISTER_MODULE(CMyNetModule)
END_REGISTER_MODULES