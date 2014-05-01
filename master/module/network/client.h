#ifndef _klib_network_client_h_
#define _klib_network_client_h_



class client
{
public: 
    static enum EVENT
    {
        EV_CONNECT,
        EV_DISCONNECT,
        EV_RECV,
        EV_SEND
    };

    void send(const char * buff, int len);
    void recv(char * buff, int len);

    void close();

    bool on_recved(const char* buff, uint32_t len) ;
    bool on_sended(const char* buff, uint32_t len) ;

    void OnConnect();
    void OnDisconnect();

public:
    uint32_t ip;
    uint16 port;

    void * srv;

    HANDLE iocp;
    EVENT event;
    SOCKET fd;
};

#endif // _klib_network_client_h_