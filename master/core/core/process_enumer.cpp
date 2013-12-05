//#include "StdAfx.h"
#include "process_enumer.h"

using namespace klib::core;


process_enumer::process_enumer(void)
{
    m_hProcessSnap = INVALID_HANDLE_VALUE;
    m_bFirst = TRUE;

    reset();
}

process_enumer::~process_enumer(void)
{
    if (INVALID_HANDLE_VALUE != m_hProcessSnap) 
    {
        CloseHandle(m_hProcessSnap);
        m_hProcessSnap = INVALID_HANDLE_VALUE;
    }
}

BOOL process_enumer::reset()
{
    m_bFirst = TRUE;
    if (m_hProcessSnap != INVALID_HANDLE_VALUE) 
    {
        CloseHandle(m_hProcessSnap);
    }
    m_hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    return m_hProcessSnap != INVALID_HANDLE_VALUE;
}

BOOL process_enumer::get_next_process_info(PROCESSENTRY32 &info)
{
    info.dwSize = sizeof(PROCESSENTRY32);
    if (m_bFirst) 
    {
        m_bFirst = FALSE;

        if(::Process32First(m_hProcessSnap, &info)) 
        {
            return TRUE;
        }
        return FALSE;
    }
    else 
    {
        return ::Process32Next(m_hProcessSnap, &info);
    }

    return FALSE;
}

VOID process_enumer::for_each(std::function<void(PROCESSENTRY32 &)> fun)
{
    PROCESSENTRY32 info;

    while (get_next_process_info(info)) 
    {
        fun(info);
    }
}