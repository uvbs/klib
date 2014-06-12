#ifndef _klib_net_socket_h
#define _klib_net_socket_h


//----------------------------------------------------------------------
//  ·â×°Ì×½Ó×Ö
class net_socket
{
public:
    net_socket(void);
    ~net_socket(void);

public:
    SOCKET& Socket() { return m_socket; }
    SOCKET& operator() () { return Socket(); }
    net_socket& operator = (SOCKET tsocket) { m_socket = tsocket; return *this; }
    operator SOCKET& () { return m_socket; }
    operator HANDLE () { return (HANDLE)m_socket; }

protected:
    SOCKET m_socket;
};

#endif
