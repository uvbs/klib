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
@desc base on TCP, use Windows IOCP to implement light weight TCP service.
*/

#pragma once

/*windows IOCP implement----------------------------------------------------------------------------------------------------*/

#ifdef LANCE_WIN32

#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")


namespace lance{ namespace net{

namespace iocp
{
WSADATA wsadata;
const int wsaversion = WSAStartup(MAKEWORD(2,2), &wsadata);

template<typename T>
class Scheduler
{
public: void start();
public: void stop();

public: void push(T * clt);

public: int scheds;

public: HANDLE iocp;
};

template<typename T>
void Scheduler<T>::start()
{
	iocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, scheds);
	if (NULL == iocp)
	{
		throw (int)::WSAGetLastError();
	}
}

template<typename T>
void Scheduler<T>::stop()
{
}

template<typename T>
void Scheduler<T>::push(T * clt)
{
	::PostQueuedCompletionStatus(iocp, 0, (ULONG_PTR)clt, NULL);
}

template<typename T>
class Processor
{
public: void start();
public: void stop();

public: int threads;
public: Scheduler<T> * scheder;

public: static DWORD WINAPI run(LPVOID param);
};

template<typename T>
void Processor<T>::start()
{
	for (int i = 0;i < threads;i ++)
	{
		DWORD tid;
		HANDLE thd = ::CreateThread(NULL,
									0,
									(LPTHREAD_START_ROUTINE)run,
									this,
									0,
									&tid);
		if (NULL == thd)
		{
			throw (int)::GetLastError();
		}
		::CloseHandle(thd);
	}
}

template<typename T>
void Processor<T>::stop()
{
}

template<typename T>
DWORD WINAPI Processor<T>::run(LPVOID param)
{
	Processor<T>& procor = *(Processor<T> *)param;
	Scheduler<T>& scheder = *procor.scheder;
	HANDLE iocp = scheder.iocp;

	DWORD ready;
	ULONG_PTR key;
	WSAOVERLAPPED * overlap;
	while (true)
	{
		::GetQueuedCompletionStatus(iocp, &ready, &key, (LPOVERLAPPED *)&overlap, INFINITE);

		T * clt = (T *)key;
		switch(clt->event)
		{
		case T::EV_RECV:
			{
				if (0 >= ready)
				{
					clt->event = T::EV_DISCONNECT;
					::PostQueuedCompletionStatus(iocp, 0, (ULONG_PTR)clt, NULL);
				}
				else
				{
					clt->OnRecv(ready);
				}
			}
			break;
		case T::EV_CONNECT:
			{
				if (NULL == ::CreateIoCompletionPort((HANDLE)clt->fd, iocp, (ULONG_PTR)clt, 0))
				{
					::closesocket(clt->fd);
					delete clt;
				}
				else
				{
					clt->OnConnect();
				}
			}
			break;
		case T::EV_DISCONNECT:
			{
				clt->OnDisconnect();
				::closesocket(clt->fd);
				delete clt;
			}
			break;
		case T::EV_SEND:
			break;
		}
	}

	return 0;
}

template<typename T>
class Listener
{
public: void start();
public: void stop();

public: int ip;
public: int port;
public: int backlogs;
public: void * srv;
public: Scheduler<T> * scheder;

public: static DWORD WINAPI run(LPVOID param);

public: SOCKET fd;
};

template<typename T>
void Listener<T>::start()
{
	SOCKET fd = INVALID_SOCKET;

	try
	{
		fd = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (INVALID_SOCKET == fd)
		{
			throw (int)WSAGetLastError();
		}

		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = ip;
		addr.sin_port = htons(port);
		if (SOCKET_ERROR == ::bind(fd, (struct sockaddr *)&addr, sizeof(addr)))
		{
			throw (int)::WSAGetLastError();
		}

		if (SOCKET_ERROR == ::listen(fd, backlogs))
		{
			throw (int)::WSAGetLastError();
		}

		DWORD tid;
		HANDLE thd = ::CreateThread(NULL,
									0,
									(LPTHREAD_START_ROUTINE)run,
									this,
									0,
									&tid);
		if (NULL == thd)
		{
			throw (int)::GetLastError();
		}
		::CloseHandle(thd);

		this->fd = fd;
	}
	catch(...)
	{
		if (INVALID_SOCKET != fd)
		{
			::closesocket(fd);
		}

		throw;
	}
}

template<typename T>
void Listener<T>::stop()
{
}

template<typename T>
DWORD WINAPI Listener<T>::run(LPVOID param)
{
	Listener<T>& listener = *(Listener<T> *)param;
	Scheduler<T>& scheder = *(Scheduler<T> *)listener.scheder;
	SOCKET lsfd = listener.fd;
	HANDLE iocp = scheder.iocp;

	SOCKET fd;
	struct sockaddr_in addr;
	int addrlen = sizeof(addr);
	while (true)
	{
		fd = ::WSAAccept(lsfd, (struct sockaddr *)&addr, &addrlen, NULL, NULL);
		if (INVALID_SOCKET == fd)
		{
			break;
		}

		T * clt = new T();
		clt->iocp = iocp;
		clt->fd = fd;
		clt->ip = addr.sin_addr.s_addr;
		clt->port = htons(addr.sin_port);
		clt->event = T::EV_CONNECT;
		clt->srv = listener.srv;
		scheder.push(clt);
	}

	return 0;
}

}

class Client : public IClient
{
public: static enum EVENT
		{
			EV_CONNECT,
			EV_DISCONNECT,
			EV_RECV,
			EV_SEND
		};

        void send(const char * buff, int len)
		{
			::send(fd, buff, len, 0);
		}
        void recv(char * buff, int len)
		{
			this->event = EV_RECV;

			WSAOVERLAPPED overlap;
			::memset(&overlap, 0, sizeof(overlap));
			WSABUF buf;
			buf.buf = buff;
			buf.len = len;
			DWORD ready = 0;
			DWORD flags = 0;
			if (0 != ::WSARecv(fd, &buf, 1, &ready, &flags, &overlap, NULL)
					&& WSA_IO_PENDING != WSAGetLastError())
			{
				this->event = EV_DISCONNECT;
				::PostQueuedCompletionStatus(iocp, 0, (ULONG_PTR)this, NULL);
			}
		}
public: void close()
		{
			::shutdown(fd, SD_BOTH);
		}

public: HANDLE iocp;
public: EVENT event;
public: SOCKET fd;
};

template<typename T>
class TCPSrv : public ITCPSrv
{
public: void start();
public: void stop();

public: int ip;
public: int port;
public: int backlogs;
public: int scheds;
public: int threads;

public: iocp::Scheduler<T> scheder;
public: iocp::Processor<T> procor;
public: iocp::Listener<T> listener;
};

template<typename T>
void TCPSrv<T>::start()
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

template<typename T>
void TCPSrv<T>::stop()
{
}

}}

#endif




