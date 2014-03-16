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
@desc base on TCP, use Linux epoll to implement light weight TCP service.
*/

#pragma once

/*linux epoll implement----------------------------------------------------------------------------------------------------*/

#ifdef LANCE_LINUX

#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/time.h> 
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#define EPOLL_MAX_NFDS			10000		// max sockets queried by epoll.
#define EPOLL_MAX_EVENTS		100			// max events queried by epoll.
#define EPOLL_MAX_QUEUE			1024		// max events in cache queue.

namespace lance{ namespace net{

namespace epoll
{
template<typename T>
class Queue
{
public: Queue()
	{
		pthread_mutex_init(&lock, NULL);
		pthread_cond_init(&emptySignal, NULL);
		pthread_cond_init(&fullSignal, NULL);
		len = 0;
		offset =  0;
	}

public: void in()
	{
		pthread_mutex_lock(&lock);
	}
public: void out()
	{
		pthread_mutex_unlock(&lock);
	}
public: void fullNotify()
	{
		pthread_cond_signal(&fullSignal);
	}
public: void fullWait()
	{
		pthread_cond_wait(&fullSignal, & lock);
	}
public: void fullBroadcast()
	{
		pthread_cond_broadcast(&fullSignal);
	}
public: void emptyNotify()
	{
		pthread_cond_signal(&emptySignal);
	}
public: void emptyWait()
	{
		pthread_cond_wait(&emptySignal, & lock);
	}
public: void emptyBroadcast()
	{
		pthread_cond_broadcast(&emptySignal);
	}


public: bool full()
	{
		return (EPOLL_MAX_QUEUE == len);
	}
public: bool empty()
	{
		return (0 == len);
	}
public: T * pop()
	{
		T * clt = clts[offset];
		len --;
		offset = (offset+1 == EPOLL_MAX_QUEUE)?0:(offset+1);
		return clt;
	}
public: void push(T * clt)
	{
		clts[(offset+len)%EPOLL_MAX_QUEUE] = clt;
		len ++;
	}

public: T* clts[EPOLL_MAX_QUEUE];
public: int len;
public: int offset;
public: pthread_mutex_t lock;
public: pthread_cond_t emptySignal;
public: pthread_cond_t fullSignal;
};

template<typename T>
class Epoller
{
public: void start();
public: void stop();

public: static void * run(void * param);

public: Queue<T> * queue;

public: int epfd;
};

template<typename T>
void Epoller<T>::start()
{
	int epfd = -1;
	try
	{
		epfd = epoll_create(EPOLL_MAX_NFDS);
		if (-1 == epfd)
		{
			throw errno;
		}

		pthread_t tid;
		int error = pthread_create(&tid, NULL, run, this);
		if (0 != error)
		{
			throw error;
		}
		pthread_detach(tid);

		this->epfd = epfd;
	}
	catch(...)
	{
		if (-1 != epfd)
		{
			close(epfd);
		}
		throw;
	}
}

template<typename T>
void Epoller<T>::stop()
{
}

template<typename T>
void * Epoller<T>::run(void * param)
{
	Epoller<T>& eper = *(Epoller<T> *)param;
	Queue<T>& queue = *eper.queue;
	int epfd = eper.epfd;

	int nfds;
	int capcity;
	struct epoll_event events[EPOLL_MAX_EVENTS];
	struct epoll_event ev;
	while (true)
	{
		nfds = epoll_wait(epfd, events, EPOLL_MAX_EVENTS, -1);
		for (int i = 0;i < nfds;i ++)
		{
			epoll_ctl(epfd, EPOLL_CTL_DEL, ((T *)events[i].data.ptr)->fd, &ev);
		}
		for (int i = 0;i < nfds;)
		{
			queue.in();
			while (queue.full())
			{
				queue.fullWait();
			}
			capcity = EPOLL_MAX_QUEUE-queue.len;
			capcity = (capcity > nfds)?nfds:capcity;
			if (queue.empty())
			{
				if (capcity > 1)
				{
					queue.emptyBroadcast();
				}
				else
				{
					queue.emptyNotify();
				}
			}
			while (capcity > 0)
			{
				if (events[i].events&EPOLLIN)
				{
					((T *)events[i].data.ptr)->event = T::EV_RECV;
				}
				else
				{
					((T *)events[i].data.ptr)->event = T::EV_DISCONNECT;
				}
				queue.push((T *)events[i].data.ptr);
				capcity --;
				i ++;
			}
			queue.out();
		}
	}

	return NULL;
}

template<typename T>
class Scheduler
{
public: void start();
public: void stop();
public: void push(T * clt);

public: int scheds;

public: Queue<T> queue;
public: Epoller<T> * epers;
public: int epoff;
};

template<typename T>
void Scheduler<T>::start()
{
	Epoller<T> * epers = NULL;
	try
	{
		epers = new Epoller<T>[scheds];
		for (int i = 0;i < scheds;i ++)
		{
			epers[i].queue = &queue;
			epers[i].start();
		}
		this->epers = epers;
		this->epoff = 0;
	}
	catch(...)
	{
		if (NULL != epers)
		{
			delete[] epers;
		}

		throw;
	}
}

template<typename T>
void Scheduler<T>::stop()
{
}

template<typename T>
void Scheduler<T>::push(T * clt)
{
	clt->epfd = epers[epoff].epfd;
	epoff = (epoff+1 == scheds)?0:(epoff+1);
	queue.in();
	while (queue.full())
	{
		queue.fullWait();
	}
	if (queue.empty())
	{
		queue.emptyNotify();
	}
	queue.push(clt);
	queue.out();
}

template<typename T>
class Processor
{
public: void start();
public: void stop();

public: static void * run(void * param);

public: int threads;
public: Scheduler<T> * scheder;
};

template<typename T>
void Processor<T>::start()
{
	for (int i = 0;i < threads;i ++)
	{
		pthread_t tid;
		int error = pthread_create(&tid, NULL, run, this);
		if (0 != error)
		{
			throw error;
		}
		pthread_detach(tid);
	}
}

template<typename T>
void Processor<T>::stop()
{
}

template<typename T>
void * Processor<T>::run(void * param)
{
	Processor<T>& procor = *(Processor<T> *)param;
	Queue<T>& queue = procor.scheder->queue;

	T * clt = NULL;
	int len = 0;
	while(true)
	{
		queue.in();
		while(queue.empty())
		{
			queue.emptyWait();
		}
		if (queue.full())
		{
			queue.fullNotify();
		}
		clt = queue.pop();
		queue.out();

		switch(clt->event)
		{
		case T::EV_RECV:
			{
				len = recv(clt->fd, clt->EPOLL_buff, clt->EPOLL_len, 0);
				if (len <= 0)
				{
					clt->OnDisconnect();
					close(clt->fd);
					delete clt;
				}
				else
				{
					clt->OnRecv(len);
				}
			}
			break;
		case T::EV_CONNECT:
			clt->OnConnect();
			break;
		case T::EV_DISCONNECT:
			clt->OnDisconnect();
			close(clt->fd);
			delete clt;
			break;
		}
	}

	return NULL;
}

template<typename T>
class Listener
{
public: void start();
public: void stop();

public: static void * run(void * param);

public: int ip;
public: int port;
public: int backlogs;
public: void * srv;
public: Scheduler<T> * scheder;

public: int fd;
};

template<typename T>
void Listener<T>::start()
{
	int fd = -1;
	try
	{
		fd = socket(AF_INET, SOCK_STREAM, 0);
		if (-1 == fd)
		{
			throw errno;
		}

		struct sockaddr_in addr;
		bzero(&addr, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = ip;
		addr.sin_port = htons(port);

		if (-1 == bind(fd, (struct sockaddr *)&addr, sizeof(addr)))
		{
			throw errno;
		}
		if (-1 == listen(fd, backlogs))
		{
			throw errno;
		}

		pthread_t tid;
		int error = pthread_create(&tid, NULL, run, this);
		if (0 != error)
		{
			throw error;
		}
		pthread_detach(tid);

		this->fd = fd;
	}
	catch(...)
	{
		if (-1 != fd)
		{
			close(fd);
		}
		throw;
	}
}

template<typename T>
void Listener<T>::stop()
{
}

template<typename T>
void * Listener<T>::run(void * param)
{
	Listener<T>& listener = *(Listener<T> *)param;
	Scheduler<T>& scheder = *listener.scheder;

	int fd;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	while (true)
	{
		fd = accept(listener.fd, (struct sockaddr *)&addr, &addrlen);
		if (-1 == fd)
		{
			break;
		}
		
		T * clt = new T();
		clt->fd = fd;
		clt->event = T::EV_CONNECT;
		clt->ip = addr.sin_addr.s_addr;
		clt->port = htons(addr.sin_port);
		clt->srv = listener.srv;

		scheder.push(clt);
	}

	return NULL;
}

}

class Client: public IClient
{
public: enum EVENT
	{
		EV_CONNECT,
		EV_DISCONNECT,
		EV_RECV
	};

public: void send(const char * buff, int len)
		{
			::send(fd, buff, len, 0);
		}
public: void recv(char * buff, int len)
		{
			this->EPOLL_buff = buff;
			this->EPOLL_len = len;

			struct epoll_event ev;
			ev.data.ptr = this;
			ev.events = EPOLLIN;
			epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
		}
public: void close()
		{
			shutdown(fd, SHUT_RDWR);
		}

public: int fd;
public: int epfd;
public: EVENT event;
public: char * EPOLL_buff;
public: int EPOLL_len;
};


template<typename T>
class TCPSrv : public ITCPSrv
{
public: TCPSrv()
	{
		ip = 0;
		port = 1234;
		ptr = NULL;
		backlogs = 10;
		threads = 1;
		scheds = 1;
	}

public: void start();
public: void stop();

public: epoll::Listener<T> listener;
public: epoll::Scheduler<T> scheder;
public: epoll::Processor<T> procor;
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
	if (scheds <= 0)
	{
		scheds = 1;
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

