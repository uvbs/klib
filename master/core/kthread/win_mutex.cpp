
//#include "stdafx.h"
#include "win_mutex.h"

#include <windows.h>

using namespace klib::kthread;


win_mutex::win_mutex() : m_hMutex(NULL)
{
}

win_mutex::~win_mutex()
{
    if (m_hMutex) 
    {
        CloseHandle(m_hMutex);
    }
}

BOOL win_mutex::is_exist_mutex(LPCTSTR lpName)
{
    HANDLE hMutex = ::OpenMutex(NULL, FALSE, lpName);
    if (NULL == hMutex) 
    {
        return FALSE;
    }
    CloseHandle(hMutex);
    return TRUE;
}

BOOL win_mutex::create_mutex(BOOL bInitialOwner, LPCTSTR lpName)
{
    m_hMutex = ::CreateMutex(NULL, bInitialOwner, lpName);

    return m_hMutex != NULL;
}

BOOL win_mutex::open_mutex(LPCTSTR lpName)
{
    m_hMutex = ::OpenMutex(NULL, FALSE, lpName);

    return m_hMutex != NULL;
}

void win_mutex::lock()
{
    DWORD dwWait = 0;
    dwWait = WaitForSingleObject(m_hMutex, INFINITE);

}

void win_mutex::unlock()
{
    ReleaseMutex(m_hMutex);
}

