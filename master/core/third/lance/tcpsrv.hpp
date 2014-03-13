/***

Copyright 2006 bsmith@qq.com

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

/**
@author bsmith.zhao, bsmith@qq.com
@date 2005-10-12
@version 0.1 network tcp service framework.
@desc network tcp service spec and interface.
*/

#pragma once


namespace lance{ namespace net{

/**
tcp service client class.
this object will be created and destoried by TCPSrv object.
*/
class IClient
{
/**
send data to client.
@param buff data bytes array.
@param len the data length in bytes.
NOTE: send is sync routine in this version, it wait until send successfully.
*/
public: virtual void send(const char * buff, int len) = 0;
/**
trigger recv routine.
@param buff data bytes array.
@param len the data length in bytes.
after trigger successfully, you can't do other system call.
because there may be other thread operates on this client.
any data copied to buff, system will call OnRecv routine with actual data recv len.
*/
public: virtual void recv(char * buff, int len) = 0;
/**
close client connection.
this close just trigger a shutdown routine, after successfully closed,
system will call OnDisconnect routine.
you must trigger a recv routine to accomplish the event chain.
*/
public: virtual void close() = 0;

/**
after connection established.
*/
public: virtual void OnConnect(){};
/**
after connection disconnected.
*/
public: virtual void OnDisconnect(){};
/**
after any data received from network.
@param len the actual data length.
*/
public: virtual void OnRecv(int len){};
/**
after any data sent to network.
@param len the actual data length.
NOTE: in this version, async send is not implemented, so this routine will nerver be called.
*/
public: virtual void OnSend(int len){};

/**
the client IP address in integer format.
*/
public: int ip;
/**
the client port.
*/
public: int port;
/**
ITCPSrv object pointer.
*/
public: void * srv;

};

class ITCPSrv
{
/**
start tcp service.
@throw int the error NO. when error occurs.
*/
public: virtual void start() = 0;
/**
stop tcp service.
NOTE: not implement.
*/
public: virtual void stop() = 0;

/**
tcp service bound ip address in integer format.
*/
public: int ip;
/**
tcp service bound port.
*/
public: int port;
/**
tcp service listener max waitting queue length.
*/
public: int backlogs;
/**
the schedule thread count.
0 indicates system auto choose.
*/
public: int scheds;
/**
the process thread count, these threads is used to process actual logic so that developer
don't need to create thread pool for logic process again.
*/
public: int threads;
/**
service bound object pointer, this is useful if you want to access outter object in logic process.
*/
public: void * ptr;

};

}}


#include "iocptcpsrv.hpp"
#include "eptcpsrv.hpp"
#include "kqtcpsrv.hpp"


