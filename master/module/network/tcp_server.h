#ifndef _klib_network_tcp_server_h_
#define _klib_network_tcp_server_h_



#include "listener.h"
#include "scheduler.h"
#include "processor.h"

class tcp_server 
{
public: 
    void start();
    void stop();

    int ip;
    int port;
    int backlogs;
    int scheds;
    int threads;

    scheduler scheder;
    processor procor;
    listener  listener;
};


#endif // _klib_network_tcp_server_h_