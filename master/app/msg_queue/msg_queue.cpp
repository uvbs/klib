// msg_queue.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "zmq.h"

#include <iostream>
#include <time.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <cstdint>

#include "zhelpers.h"

#pragma comment(lib, "libzmq_d")

int randof(int v)
{
    int r = rand();
    r %= v;

    return r;
}

/*
void reciver()
{

    std::cout << "———Test Client Start… (C) www.cppfans.org———" <<std::endl;
    std::cout << "———————Author:eliteYang———————" <<std::endl;

    void* context = zmq_init( 1 );
    void* responder = zmq_socket( context, ZMQ_REP );
    zmq_bind( responder, "tcp://*:5555" );

    char buff[1024];

    while ( true )
    {
        //zmq_msg_t msg;
        //zmq_msg_init( &msg );
        //zmq_recvmsg( responder, &msg, 0 );
        //zmq_recv(responder, buff, _countof(buff), 0);

        //zmq_msg_t msg;
        //zmq_msg_init( &msg );
        //zmq_msg_recv(&msg, responder, 0);

        int ret = zmq_recv(responder, buff, _countof(buff), 0);
        if (ret > 0) {
            buff[ret] = '\0';
            std::cout << "Recv " << buff << " from Client" << std::endl;
        }

        zmq_send(responder, "World", 5, 0);

        //zmq_msg_close( &msg );

        Sleep( 100 );
    }
    zmq_close (responder);
    zmq_term (context);
    system( "Pause" );
}

void sender()
{
    std::cout << "———Test Client Start… (C) www.cppfans.org———" <<std::endl;
    std::cout << "———————Author:eliteYang———————" <<std::endl;

    void* context = zmq_init( 1 );
    void* responder = zmq_socket( context, ZMQ_REQ );
    int nRet = zmq_connect( responder, "tcp://localhost:5555" );
    if ( nRet == 0 )
    { std::cout << "—————-Client Connect Server OK——————" <<std::endl; }

    char buff[1024];

    for ( int i = 0; i < 10; ++i )
    {
        
        //zmq_msg_t msg;
        //zmq_msg_init_size( &msg, 32 );
        //memcpy( zmq_msg_data( &msg ), "Hello world!", 15);
        //zmq_sendmsg( responder, &msg, 0 );
        //zmq_msg_send(&msg, responder, 0);
        //zmq_msg_close( &msg );
        //std::cout << "Send Msg " << i << " Times" << std::endl;
        

        zmq_send(responder, "Hello", 5, 0);
        int ret = zmq_recv(responder, buff, _countof(buff), 0);
        if (ret > 0) {
            buff[ret] = '\0';
        }
        printf("%s \n", buff);
    }

    zmq_close (responder);
    zmq_term (context);
    system( "Pause" );
}

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc < 2) {
        return 0;
    }

    if (_tcscmp(argv[1], _T("recv")) == 0) 
    {
        reciver();
    }
    else if (_tcscmp(argv[1], _T("send")) == 0) 
    {
        sender();
    }

    return 0;
}

*/


/*
void subscriber()
{
    //void *m_context;
    //void *m_subscriber;
    //m_context = zmq_init(1);
    //m_subscriber = zmq_socket(m_context,ZMQ_SUB);

    //zmq_connect(m_subscriber, "tcp://127.0.0.1:8585");

    //char *option = "642";
    //int ret = zmq_setsockopt(m_subscriber, ZMQ_SUBSCRIBE, "642", strlen(option));

    //while (1)
    //{
    //    BYTE buffer[1024] = {0};
    //    DWORD bufLen = sizeof(buffer);
    //    int gotLen = zmq_recv(m_subscriber, buffer, bufLen, 0);
    //    if (gotLen > 0) {
    //        buffer[gotLen] = 0;
    //    }
    //    printf("recvd: %s\n", buffer);
    //}

    //zmq_close(m_subscriber);
    //zmq_term(m_context);

    void* ctx = zmq_ctx_new();
    void* subscriber = zmq_socket(ctx,ZMQ_SUB);
    zmq_connect(subscriber,"tcp://localhost:5556");

    char* filter =  "6 -";
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filter, strlen(filter));
    long total_temp = 0;
    int update_cnt;
    for(update_cnt = 0; update_cnt < 100; ++update_cnt){
        char* report = s_recv(subscriber);
        printf("收到天气预报: %s\n",report);

        int zip,tmp,rel;
        sscanf(report,"%d %d %d",&zip,&tmp,&rel);
        total_temp += tmp;
        free(report);
    }
    printf("邮编: %s 平均温度: %d\n", filter, (int)(total_temp / update_cnt));
    zmq_close(subscriber);
    zmq_ctx_destroy(ctx);
}

void publisher()
{
    //void *m_context;
    //void *m_publisher;
    
   // m_context = zmq_init(1);
    //m_publisher = zmq_socket(m_context,  ZMQ_PUB);
    //zmq_bind(m_publisher, "tcp://*:8585");
    
    //char *msg = "test|1234555testss";
    //while (1)
    //{
    //    int sendlen = zmq_send(m_publisher, msg, strlen(msg), 0);
     //   printf("send: %s\n",msg);
     //   
    //    Sleep(1000);
    //}
    
    //zmq_close(m_publisher);
    //zmq_term(m_context);

    void* ctx = zmq_ctx_new();
    void* publisher = zmq_socket(ctx, ZMQ_PUB);
    zmq_bind(publisher,"tcp://*:5556");
    srand((unsigned int)time(NULL));
    while(1)
    {
        int zipcode,temperature,relhumidity;
        zipcode = randof(10);
        temperature = randof(215) - 80;
        relhumidity = randof(50) + 10;
        char report[20];

        _snprintf(report, 20, "%d %d %d",zipcode,temperature,relhumidity);
        printf("发布天气预报: %s\n",report);
        s_send(publisher,report);
    }

    zmq_close(publisher);
    zmq_ctx_destroy(ctx);
}

int _tmain(int argc, _TCHAR* argv[])
{
    setlocale(LC_ALL,"Chinese");
    setlocale(LC_ALL,"chs");

    if (argc < 2) {
        return 0;
    }

    if (_tcscmp(argv[1], _T("pub")) == 0) 
    {
        publisher();
    }
    else if (_tcscmp(argv[1], _T("sub")) == 0) 
    {
        subscriber();
    }
    
    return 0;
}

*/


/*
// 发布代理
#include <zmq.h>
#include <zmq_helpers.h>
#include <stdio.h>

int main(void)
{
    void* ctx = zmq_ctx_new();
    void* frontend = zmq_socket(ctx,ZMQ_SUB);
    zmq_setsockopt(frontend,ZMQ_SUBSCRIBE,"",0);
    zmq_connect(frontend,"tcp://localhost:5556");
    void* backend = zmq_socket(ctx,ZMQ_PUB);
    zmq_bind(backend,"tcp://*:8100");
    while(true){        
        zmq_msg_t report;
        zmq_msg_init(&report);
        zmq_recvmsg(frontend,&report,0);    
        int hasmore;
        size_t optlen = sizeof(hasmore);
        zmq_getsockopt(frontend,ZMQ_RCVMORE,&hasmore,&optlen);
        zmq_sendmsg(backend,&report,((hasmore == 0) ? 0 : ZMQ_SNDMORE));        
        zmq_msg_close(&report);
    }    
    zmq_close(frontend);
    zmq_close(backend);
    zmq_ctx_destroy(ctx);
    return 0;
}

// 客户端
#include <zmq.h>
#include <zmq_helpers.h>
#include <stdio.h>
#pragma comment(lib,"libzmq-v100-mt.lib")
int main(void){
void* ctx = zmq_ctx_new();
void* client = zmq_socket(ctx,ZMQ_REQ);
zmq_connect(client,"tcp://localhost:5559");
s_sleep(500);
for(int idx = 0; idx < 10; ++idx){
zmq_msg_t request;
zmq_msg_init_size(&request,6);
memcpy(zmq_msg_data(&request),"Hello",6);
printf("发送Hello\n");
zmq_sendmsg(client,&request,0);
zmq_msg_close(&request);
zmq_msg_t reply;
zmq_msg_init(&reply);
zmq_recvmsg(client,&reply,0);
printf("收到%s\n",(char*)zmq_msg_data(&reply));
zmq_msg_close(&reply);
}
zmq_close(client);
zmq_ctx_destroy(ctx);
return 0;
}


// 服务端
#include <zmq.h>
#include <zmq_helpers.h>
#include <stdio.h>
#pragma comment(lib,"libzmq-v100-mt.lib")
int main(void){
    void* ctx = zmq_ctx_new();
    void* responder = zmq_socket(ctx,ZMQ_REP);
    zmq_connect(responder,"tcp://localhost:5560");
    s_sleep(1000);
    while(1)
    {
        zmq_msg_t request;
        zmq_msg_init(&request);
        zmq_recvmsg(responder,&request,0);
        printf("收到%s\n",(char*)zmq_msg_data(&request));
        zmq_msg_close(&request);
        s_sleep(1000);
        zmq_msg_t reply;
        zmq_msg_init_size(&reply,6);
        memcpy(zmq_msg_data(&reply),"World",6);
        printf("发送World\n");
        zmq_sendmsg(responder,&reply,0);
        zmq_msg_close(&reply);
    }
    zmq_close(responder);
    zmq_ctx_destroy(ctx);
    return 0;
}

// 代理
#include <zmq.h>
#include <zmq_helpers.h>
#include <stdio.h>
#pragma comment(lib,"libzmq-v100-mt.lib")
int main(void){
    void* ctx = zmq_ctx_new();
    void* frontend = zmq_socket(ctx,ZMQ_ROUTER);
    zmq_bind(frontend,"tcp://*:5559");
    void* backend = zmq_socket(ctx,ZMQ_DEALER);
    zmq_bind(backend,"tcp://*:5560");
    zmq_pollitem_t items[]={
        {frontend,0,ZMQ_POLLIN,0},
        {backend,0,ZMQ_POLLIN,0},
    };
    while(zmq_poll(items,2,-1) >= 0){
        // router --> dealer
        if (items[0].revents & ZMQ_POLLIN){
            while(true){
                zmq_msg_t a_msg;
                zmq_msg_init(&a_msg);
                zmq_recvmsg(frontend,&a_msg,0);
                int hasmore;
                size_t optlen = sizeof(hasmore);
                zmq_getsockopt(frontend,ZMQ_RCVMORE,&hasmore,&optlen);
                zmq_sendmsg(backend,&a_msg,((hasmore == 0) ? 0 : ZMQ_SNDMORE));
                zmq_msg_close(&a_msg);
                if (!hasmore){
                    break;
                }
            }
        }
        // dealer --> router
        if (items[1].revents & ZMQ_POLLIN){
            while(true){
                zmq_msg_t a_msg;
                zmq_msg_init(&a_msg);
                zmq_recvmsg(backend,&a_msg,0);
                int hasmore;
                size_t optlen = sizeof(hasmore);
                zmq_getsockopt(backend,ZMQ_RCVMORE,&hasmore,&optlen);
                zmq_sendmsg(frontend,&a_msg,((hasmore == 0) ? 0 : ZMQ_SNDMORE));
                zmq_msg_close(&a_msg);
                if (!hasmore){
                    break;
                }
            }
        }
    }
    
    zmq_close(frontend);
    zmq_close(backend);
    zmq_ctx_destroy(ctx);
    return 0;
}


*/


/*
// 内建设置做转发
// ZeroMQ提供了三种内建的设备：
// QUEUE 用作请求-应答代理，要求ROUTER/DEALER套接字对 
// FORWARDER 用作发布-订阅代理，要求PUB/SUB套接字对 
// STREAMER 与FORWARDER相似，只是用于管线流程，要求PULL/PUSH套接字对 


#include <zmq.h>
#include <zmq_helpers.h>
#include <stdio.h>
#pragma comment(lib,"libzmq-v100-mt.lib")
int main(void)
{
    void* ctx = zmq_ctx_new();
    void* frontend = zmq_socket(ctx,ZMQ_ROUTER);
    zmq_bind(frontend,"tcp://*:5559");
    void* backend = zmq_socket(ctx,ZMQ_DEALER);
    zmq_bind(backend,"tcp://*:5560");
    zmq_device(ZMQ_QUEUE,frontend,backend);
    zmq_close(frontend);
    zmq_close(backend);
    zmq_ctx_destroy(ctx);
    return 0;
}
*/


/*

// 多线程版本

#include <zmq.h>
#include <zmq_helpers.h>
#include <boost/thread.hpp>
#include <stdio.h>
#pragma comment(lib,"libzmq-v100-mt.lib")
static void worker_proc(void* ctx){
    void* responder = zmq_socket(ctx,ZMQ_REP);
    zmq_connect(responder,"inproc://workers");
    while(1){
        zmq_msg_t request;
        zmq_msg_init(&request);
        zmq_recvmsg(responder,&request,0);
        printf("收到%s\n",(char*)zmq_msg_data(&request));
        zmq_msg_close(&request);
        s_sleep(1000);
        zmq_msg_t reply;
        zmq_msg_init_size(&reply,6);
        memcpy(zmq_msg_data(&reply),"World",6);
        printf("发送World\n");
        zmq_sendmsg(responder,&reply,0);
        zmq_msg_close(&reply);
    }
}
int main(void){
    void* ctx = zmq_ctx_new();
    void* frontend = zmq_socket(ctx,ZMQ_ROUTER);
    zmq_bind(frontend,"tcp://*:5559");
    void* backend = zmq_socket(ctx,ZMQ_DEALER);
    zmq_bind(backend,"inproc://workers");
    for(int idx = 0; idx < 5; ++idx){
        boost::thread worker(worker_proc,ctx);
    }
    zmq_device(ZMQ_QUEUE,frontend,backend);
    zmq_close(frontend);
    zmq_close(backend);
    zmq_ctx_destroy(ctx);
    return 0;
}
*/

static int64_t s_clock()
{
    return time(nullptr);
}

static void s_sleep(int64_t s)
{
    Sleep(s);
}

void generator()
{
    void* ctx = zmq_ctx_new();

    void* sender = zmq_socket(ctx, ZMQ_PUSH);
    zmq_bind(sender,"tcp://*:5557");

    void* sink = zmq_socket(ctx, ZMQ_PUSH);
    zmq_connect(sink,"tcp://localhost:5558");

    printf("请在工作者就绪后按回车...");
    getchar();
    printf("开始向工作者发送任务...\n");

    // 通知汇聚点,即将开始工作,请准备汇集任务执行结果
    s_send(sink, "0");
    srand((unsigned int)time(NULL));
    int task_cnt;
    int total_ms = 0;
    for(task_cnt = 0; task_cnt < 100; ++task_cnt)
    {
        int workload = randof(100) + 1;
        total_ms += workload;
        
        char szwork[10];
        _snprintf(szwork, 10, "%d", workload);
        
        printf("M ",workload);
        if ((task_cnt + 1) % 10 == 0){
            printf("\n");
        }
        fflush(stdout);
        s_send(sender, szwork);
    }

    printf("预期开销: %d 毫秒\n",total_ms);
    s_sleep(1000);
    zmq_close(sink);
    zmq_close(sender);
    zmq_ctx_destroy(ctx);
}

void worker()
{
    void* ctx = zmq_ctx_new();
    void* receiver = zmq_socket(ctx, ZMQ_PULL);
    zmq_connect(receiver,"tcp://localhost:5557");

    void* sender = zmq_socket(ctx, ZMQ_PUSH);
    zmq_connect(sender,"tcp://localhost:5558");
    while(1)
    {
        // 接收任务
        char* p_work = s_recv(receiver);
        fflush(stdout);
        printf("%s\n",p_work);

        // 执行任务
        s_sleep(atoi(p_work));
        free(p_work);

        // 通知汇聚点已经完成一个任务
        s_send(sender, "");
    }

    zmq_close(receiver);
    zmq_close(sender);
    zmq_ctx_destroy(ctx);
}

void sinker()
{
    void* ctx = zmq_ctx_new();
    void* sink = zmq_socket(ctx, ZMQ_PULL);
    zmq_bind(sink,"tcp://*:5558");

    // 等待任务发生器通知开始
    char* p_start = s_recv(sink);
    free(p_start);
    int64_t start_time = s_clock();
    printf("开始收集任务执行结果\n");

    // 收集个任务执行结果
    int task_cnt;
    for(task_cnt = 0; task_cnt < 100; ++task_cnt)
    {
        char* p_done = s_recv(sink);
        free(p_done);
        printf(".");
        if ((task_cnt + 1) % 10 == 0)
        {
            printf("\n");
        }
        fflush(stdout);
    }

    printf("100个任务实际耗时: %d 毫秒\n", int(s_clock() - start_time));
    zmq_close(sink);
    zmq_ctx_destroy(ctx);
}

int _tmain(int argc, _TCHAR* argv[])
{
    setlocale(LC_ALL,"Chinese");
    setlocale(LC_ALL,"chs");

    if (argc < 2) {
        return 0;
    }

    if (_tcsncmp(argv[1], _T("gen"), 3) == 0) 
    {
        generator();
    }
    else if (_tcsncmp(argv[1], _T("worker"), 3) == 0) 
    {
        worker();
    }
    else if (_tcsncmp(argv[1], _T("sinker"), 3) == 0) 
    {
        sinker();
    }

    return 0;
}