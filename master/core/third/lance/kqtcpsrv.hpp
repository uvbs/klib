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
@desc base on TCP, use FreeBSD kqueue to implement light weight TCP service.
*/

#pragma once

/*freebsd kequeue implement----------------------------------------------------------------------------------------------------*/

#ifdef LANCE_FREEBSD

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#define KQUEUE_MAX_NFDS			10000		// max sockets queried by kqueue.
#define KQUEUE_MAX_EVENTS		100			// max events queried by kqueue.
#define KQUEUE_MAX_QUEUE		1024		// max events in cache queue.

namespace lance{ namespace net{

namespace kqueue
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
		return (KQUEUE_MAX_QUEUE == len);
	}
public: bool empty()
	{
		return (0 == len);
	}
public: T * pop()
	{
		T * clt = clts[offset];
		len --;
		offset = (offset+1 == KQUEUE_MAX_QUEUE)?0:(offset+1);
		return clt;
	}
public: void push(T * clt)
	{
		clts[(offset+len)%KQUEUE_MAX_QUEUE] = clt;
		len ++;
	}

public: T* clts[KQUEUE_MAX_QUEUE];
public: int len;
public: int offset;
public: pthread_mutex_t lock;
public: pthread_cond_t emptySignal;
public: pthread_cond_t fullSignal;
};

template<typename T>
class KQueuer
{
public: void start();
public: void stop();

public: static void * run(void * param);

public: Queue<T> * queue;

public: int kqfd;
};

template<typename T>
void KQueuer<T>::start()
{
	int kqfd = -1;

	try
	{
		kqfd = ::kqueue();
		if (-1 == kqfd)
		{
			throw (int)errno;
		}

		pthread_t tid;
		int error = pthread_create(&tid, NULL, run, this);
		if (0 != error)
		{
			throw error;
		}
		pthread_detach(tid);

		this->kqfd = kqfd;
	}
	catch(...)
	{
		if (-1 != kqfd)
		{
			close(kqfd);
		}
		throw;
	}
}

template<typename T>
void * KQueuer<T>::run(void * param)
{
	KQueuer<T>& kqer = *(KQueuer<T> *)param;
	Queue<T>& queue = *kqer.queue;
	int kqfd = kqer.kqfd;

	int nfds;
	int capcity;
	struct kevent events[KQUEUE_MAX_EVENTS];
	struct kevent ev;
	while (true)
	{
		nfds = kevent(kqfd, NULL, 0, events, KQUEUE_MAX_EVENTS, NULL);
		for (int i = 0;i < nfds;i ++)
		{
			EV_SET(&ev, events[i].ident, EVFILT_READ, EV_DELETE, 0, 0, 0);
			kevent(kqfd, &ev, 1, NULL, 0, NULL);
		}
		for (int i = 0;i < nfds;)
		{
			queue.in();
			while (queue.full())
			{
				queue.fullWait();
			}
			capcity = KQUEUE_MAX_QUEUE-queue.len;
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
				((T *)events[i].udata)->event = T::EV_RECV;
				queue.push((T *)events[i].udata);
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
public: KQueuer<T> * kqers;
public: int kqoff;
};

template<typename T>
void Scheduler<T>::start()
{
	kqers = NULL;
	try
	{
		kqers = new KQueuer<T>[scheds];
		for (int i = 0;i < scheds;i ++)
		{
			kqers[i].queue = &queue;
			kqers[i].start();
		}

		this->kqers = kqers;
		this->kqoff = 0;
	}
	catch(...)
	{
		if (NULL != kqers)
		{
			delete[] kqers;
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
	clt->kqfd = kqers[kqoff].kqfd;
	kqoff = (kqoff+1 == scheds)?0:(kqoff+1);
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
				len = recv(clt->fd, clt->KQUEUE_buff, clt->KQUEUE_len, 0);
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
			this->KQUEUE_buff = buff;
			this->KQUEUE_len = len;

			struct kevent ev;
			EV_SET(&ev, fd, EVFILT_READ, EV_ADD|EV_ENABLE, 0, 0, this);
			kevent(kqfd, &ev, 1, NULL, 0, NULL);
		}
public: void close()
		{
			shutdown(fd, SHUT_RDWR);
		}

public: int fd;
public: int kqfd;
public: EVENT event;
public: char * KQUEUE_buff;
public: int KQUEUE_len;
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

public: kqueue::Listener<T> listener;
public: kqueue::Scheduler<T> scheder;
public: kqueue::Processor<T> procor;
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


