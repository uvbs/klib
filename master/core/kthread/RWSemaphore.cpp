#include "RWSemaphore.h"
#include <tchar.h>

namespace klib {
namespace kthread {


CWRSemaphore::CWRSemaphore(tstring strSeamporeName):
    m_write(1, 1, strSeamporeName + _T("write")), 
    m_read(0, 1, strSeamporeName + _T("read"))
{
}

CWRSemaphore::~CWRSemaphore(void)
{
}

void CWRSemaphore::write_wait()
{
	m_write.WaitSemaphore();
}

void CWRSemaphore::read_wait()
{
	m_read.WaitSemaphore();
}

void CWRSemaphore::write_set()
{
	m_write.ReleaseSemaphore();
}

void CWRSemaphore::read_set()
{
	m_read.ReleaseSemaphore();
}

void CWRSemaphore::close_semaphore()
{
	m_write.Close();
	m_read.Close();
}


}}
