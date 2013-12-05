#include "Semaphore.h"
#include <tchar.h>

namespace klib {
namespace kthread{


CSemaphore::CSemaphore(int usercount, int maxcount, tstring name)
{
	m_hSemaphore = CreateSemaphore(NULL, usercount, maxcount, name.c_str());
	if(NULL == m_hSemaphore)
	{
		MessageBox(NULL, _T("信号量创建失败！"), NULL, MB_OK);
	}
}

CSemaphore::~CSemaphore(void)
{
	Close();
}

void CSemaphore::ReleaseSemaphore(int n /*= 1*/)
{
	::ReleaseSemaphore(m_hSemaphore, n, NULL);
}

void CSemaphore::WaitSemaphore(DWORD dwMilliseconds /*= INFINITE*/ )
{
	WaitForSingleObject(m_hSemaphore, dwMilliseconds);
}

void CSemaphore::Close()
{
	CloseHandle(m_hSemaphore);
}


}}

