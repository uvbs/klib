#ifndef _klib_network_scheduler_h_
#define _klib_network_scheduler_h_



class client;

class scheduler
{
public:
    void start();
    void stop();

    void push(client * clt);

    int scheds;

    HANDLE iocp;
};



#endif // _klib_network_scheduler_h_