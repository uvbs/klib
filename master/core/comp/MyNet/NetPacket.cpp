#include "StdAfx.h"
#include "NetPacket.h"


NetPacket::NetPacket(void)
{
    m_bFixed	= false;
    pConn		= NULL;
    datalen	= 0;
}

NetPacket::~NetPacket(void)
{
}
