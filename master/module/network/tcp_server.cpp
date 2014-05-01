#include "stdafx.h"
#include "tcp_server.h"



void tcp_server::start()
{
    if (backlogs <= 0)
    {
        backlogs = 10;
    }
    if (threads <= 0)
    {
        threads = 1;
    }

    scheder.scheds = scheds;
    scheder.start();

    procor.threads = threads;
    procor.scheder = &scheder;
    procor.start();

    listener.ip = ip;
    listener.port = port;
    listener.backlogs = backlogs;
    listener.srv = this;
    listener.scheder = &scheder;
    listener.start();
}

void tcp_server::stop()
{
}


