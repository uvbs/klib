#pragma once
#include "Semaphore.h"
#include "../istddef.h"

namespace klib {
namespace kthread {


class CWRSemaphore
{
public:
	CWRSemaphore(tstring strSeamporeName);
	~CWRSemaphore(void);

	//等待可写信号
	void write_wait();

	//等待可读信号
	void read_wait();

	//设置写信号
	void write_set();

	//设置读信号
	void read_set();

	void close_semaphore();

private:
	CSemaphore      m_write;
	CSemaphore      m_read;
};


}}
