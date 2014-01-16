#include "StdAfx.h"
#include "INetConnection.h"

INetConnection::INetConnection(void)
{
  datalen = 0;
  m_PostReadCount = 0;
  m_PostWriteCount = 0;
  m_bClosing = FALSE;
  m_tLastActive = 0;
  m_PeerPort = 0;
  m_LocalPort = 0;
  bind_key_ = 0;
  m_dwBytesRead = 0;
  m_dwBytesWrite = 0;
  memset(m_strAddress, 0, sizeof(m_strAddress));
}

INetConnection::~INetConnection(void)
{
  datalen = 0;
  m_PostReadCount = 0;
  m_PostWriteCount = 0;
  m_bClosing = FALSE;
  m_tLastActive = 0;
  m_PeerPort = 0;
  m_LocalPort = 0;
  bind_key_ = 0;
  m_dwBytesRead = 0;
  m_dwBytesWrite = 0;
  memset(m_strAddress, 0, sizeof(m_strAddress));
}

bool INetConnection::InitPeerInfo()
{
	sockaddr_in sockaddr;
	memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	int len = sizeof(sockaddr);
	if (SOCKET_ERROR != getpeername(GetSocket(), (struct sockaddr*) & sockaddr, &len)) {
		SetPeerPort(ntohs(sockaddr.sin_port));
		m_PeerAddr = sockaddr.sin_addr.s_addr;
		return true;
	}

	return false;
}

// char* INetConnection::GetPeerAddress() 
// {
// 	if (m_strAddress[0] == '\0') {
// 		SetPeerAddress(inet_ntoa(*(in_addr*) &m_PeerAddr));
// 	}
// 	return m_strAddress;
// }

bool INetConnection::AddStream(const char* buff, int len) 
{
  if (len + datalen > MAX_CLIENT_BUFF_LEN) {
    return false;
  }

  memcpy(this->mbuff + datalen, buff, len);
  datalen += len;
  return true;
}

bool INetConnection::GetStream(char* buff, int len)
{
  if (len > datalen) {
    return false;
  }
  memcpy(buff, this->mbuff, len);//´æ´¢µ½Íâ²¿
  memcpy(this->mbuff, this->mbuff + len, datalen - len);
  datalen -= len;

  return true;
}

void INetConnection::UpdateLastActiveTimestamp()  
{
  m_tLastActive = GetTickCount();
}

void INetConnection::AddPostReadCount() 
{
  guard helper(mutex_);
  ++ m_PostReadCount;
}

void INetConnection::DecPostReadCount()
{
  guard helper(mutex_);
  -- m_PostReadCount;
}

USHORT INetConnection::GetPostReadCount()
{
  guard helper(mutex_);
  return m_PostReadCount;
}

void INetConnection::AddPostWriteCount()
{
  guard helper(mutex_);
  ++ m_PostWriteCount;
}

void INetConnection::DecPostWriteCount()
{
  guard helper(mutex_);
  -- m_PostWriteCount;
}

USHORT INetConnection::GetPostWriteCount()
{
  guard helper(mutex_);
  return m_PostWriteCount;
}
