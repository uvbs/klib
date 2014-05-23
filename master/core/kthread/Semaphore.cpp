#include "Semaphore.h"
#include <tchar.h>

namespace klib {
namespace kthread{


semaphore::semaphore(int usercount, int maxcount, tstring name)
{
	m_hSemaphore = CreateSemaphore(NULL, usercount, maxcount, name.c_str());
	if(NULL == m_hSemaphore)
	{
		MessageBox(NULL, _T("信号量创建失败！"), NULL, MB_OK);
	}
}

semaphore::~semaphore(void)
{
	close();
}

void semaphore::release(int n /*= 1*/)
{
	::ReleaseSemaphore(m_hSemaphore, n, NULL);
}

void semaphore::wait(DWORD dwMilliseconds /*= INFINITE*/ )
{
	WaitForSingleObject(m_hSemaphore, dwMilliseconds);
}

void semaphore::close()
{
	CloseHandle(m_hSemaphore);
}


}}

