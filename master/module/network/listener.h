#ifndef _lib_network_listener_h_
#define _lib_network_listener_h_


class scheduler;

class listener
{
public: 
    void start();
    void stop();

    static DWORD WINAPI run(LPVOID param);

    int ip;
    int port;
    int backlogs;
    void * srv;
    scheduler * scheder;

    SOCKET fd;
};





#endif // _lib_network_listener_h_

