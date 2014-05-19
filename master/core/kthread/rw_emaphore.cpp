#include "rw_semaphore.h"
#include <tchar.h>

namespace klib {
namespace kthread {


wr_semaphore::wr_semaphore(tstring strSeamporeName):
    m_write(1, 1, strSeamporeName + _T("write")), 
    m_read(0, 1, strSeamporeName + _T("read"))
{
}

wr_semaphore::~wr_semaphore(void)
{
}

void wr_semaphore::write_wait()
{
	m_write.wait();
}

void wr_semaphore::read_wait()
{
	m_read.wait();
}

void wr_semaphore::write_set()
{
	m_write.release();
}

void wr_semaphore::read_set()
{
	m_read.release();
}

void wr_semaphore::close_semaphore()
{
	m_write.close();
	m_read.close();
}


}}
