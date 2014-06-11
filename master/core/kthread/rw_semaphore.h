#ifndef _klib_rw_semaphore_h
#define _klib_rw_semaphore_h
#include "semaphore.h"
#include "../istddef.h"

namespace klib {
namespace kthread {


class wr_semaphore
{
public:
	wr_semaphore(tstring strSeamporeName);
	~wr_semaphore(void);

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
	semaphore      m_write;
	semaphore      m_read;
};


}}


#endif
