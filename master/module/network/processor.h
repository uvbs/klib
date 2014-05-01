#ifndef _klib_network_processor_h_
#define _klib_network_processor_h_


class scheduler;

class processor
{
public: 
    void start();
    void stop();

    int threads;
    scheduler * scheder;

    static DWORD WINAPI run(LPVOID param);
};


#endif // _klib_network_processor_h_