#include "StdAfx.h"
#include "net_packet.h"


net_packet::net_packet(void)
{
    m_bFixed	= false;
    pConn		= NULL;
    datalen	= 0;
}

net_packet::~net_packet(void)
{
}
