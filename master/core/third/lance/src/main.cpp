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

#include <stdio.h>

#include "./lance/ldebug.h"
#include "./lance/tcpsrv.hpp"
#include "./lance/systm.h"

// Client对象类，当连接建立时自动创建，当连接断开时自动销毁
class MyClient : public lance::net::Client
{
		// 连接建立时被调动
public: void OnConnect()
		{
			printf("OnConnect: fd=%08x, ip=%d, port=%d\n", fd, ip, port);
			// 通知调度系统接收数据
			// 数据这时并没有真正接收，当客户端有数据发送来时
			// 调度器自动接收数据，然后通过OnRecv通知数据接收完成
			recv(data, 255);
		}
		// 连接断开时被调用
public: void OnDisconnect()
		{
			printf("OnDisconnect: fd=%08x, ip=%d, port=%d\n", fd, ip, port);
		}
		// 当有数据被接收时调用，接收的实际数据长度为len
public: void OnRecv(int len)
		{
			data[len] = 0x00;
			printf("OnRecv: fd=%08x, data=%s\n", fd, data);
			// 断开连接命令
			if (data[0] == 'a')
			{
				printf("user exit command\n");
				// 通知调度系统断开连接，当调度系统处理完成后才真正断开连接
				close();
			}
			// 通知调度系统接收数据
			// 数据这时并没有真正接收，当客户端有数据发送来时
			// 调度器自动接收数据，然后通过OnRecv通知数据接收完成
			recv(data, 255);
		}
// 数据缓冲区
public: char data[256];
};


int main(char * args[])
{
	lance::net::TCPSrv<MyClient> srv;
	// 设置监听套接字绑定的IP
	// 0为绑定所有本机可用IP地址
	srv.ip = 0;
	// 监听端口
	srv.port = 1234;
	// 绑定的对象或资源指针
	// MyClient里面可以通过srv->ptr获取这个指针
	srv.ptr = NULL;
	// 监听套接字连接队列长度
	srv.backlogs = 10;
	// 处理线程池线程数
	srv.threads = 1;
	// 调度器线程数，通常是本机CPU数的2倍
	// 0表示自动选择
	srv.scheds = 0;
	// 启动网络服务
	srv.start();

	// 循环，保证进程不退出
	while(true)
	{
		lance::systm::sleep(2000);
	}

	return 0;
}
