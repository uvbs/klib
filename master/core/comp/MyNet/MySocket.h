#pragma once


//----------------------------------------------------------------------
// Summary:
//      ·â×°Ì×½Ó×Ö
//----------------------------------------------------------------------

class NetSocket
{
public:
  NetSocket(void);
  ~NetSocket(void);

public:
  SOCKET& Socket() { return m_socket; }
  SOCKET& operator() () { return Socket(); }
  NetSocket& operator = (SOCKET tsocket) { m_socket = tsocket; return *this; }
  operator SOCKET& () { return m_socket; }
  operator HANDLE () { return (HANDLE)m_socket; }

protected:
  SOCKET m_socket;
};