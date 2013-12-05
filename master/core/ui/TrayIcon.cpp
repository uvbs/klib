#include "stdafx.h"
#include "TrayIcon.h"
#include <windows.h>

using namespace klib;
using namespace klib::ui;

/********************************************************************** 
* File Name: TrayIcon.cpp 
* Module Name: CTrayIcon 
* Copyright (C) by LQ messager 
* Description: System taskbar tray icon control 
* Author: LiJun 
* Create Data: 2005-11-18   Update Data: 2006-07-18 
* E-Mail: mailto:yesme2000@qq.com 
* 
*********************************************************************/ 
#include "trayicon.h " 

CTrayIcon::CTrayIcon() 
{ 
    memset(&m_nid, 0, sizeof(NOTIFYICONDATA));
    m_nid.cbSize = sizeof(NOTIFYICONDATA);
} 

CTrayIcon::~CTrayIcon() 
{
    ShowTray(FALSE);
}

BOOL CTrayIcon::SetNotifyWnd(HWND hWnd, UINT nMsg)
{ 
    _ASSERT(hWnd); 
    _ASSERT(::IsWindow(hWnd));

    m_nid.hWnd  = hWnd; 
    if(nMsg > 0) 
    { 
        m_nid.uCallbackMessage = nMsg;
        m_nid.uFlags |= NIF_MESSAGE;
    } 
    else 
    {
        m_nid.uFlags &= ~NIF_MESSAGE; 
    } 
    return TRUE; 
} 

BOOL CTrayIcon::SetIcon(HICON hIcon) 
{ 
    _ASSERT(hIcon);

    m_nid.uFlags |= NIF_ICON;
    m_nid.hIcon = hIcon;

    return Shell_NotifyIcon(NIM_MODIFY, &m_nid);
} 

BOOL CTrayIcon::SetIcon(UINT nID)
{ 
    return SetIcon(LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(nID)));
} 

HICON CTrayIcon::GetIcon() const
{ 
    return m_nid.hIcon;
} 

BOOL CTrayIcon::SetToolTip(LPCTSTR  pszTip)
{
    m_nid.uFlags |= NIF_TIP;
    _tcsncpy(m_nid.szTip,  pszTip, _countof(m_nid.szTip));

    return Shell_NotifyIcon(NIM_MODIFY, &m_nid);
} 

BOOL CTrayIcon::SetToolTip(UINT nID)
{ 
    TCHAR szText[128];
    ::LoadString(GetModuleHandle(NULL), nID, szText, 128);
    return SetToolTip(szText);
} 

LPCTSTR CTrayIcon::GetToolTip() const
{ 
    return m_nid.szTip;
} 

#if (_WIN32_IE >= 0x0500)
BOOL CTrayIcon::ShowBalloonTip(LPCTSTR pszTitle, LPCTSTR  pszMsg, UINT nTimeout)
{ 
    m_nid.uFlags |= NIF_INFO;
    m_nid.uTimeout =  nTimeout;
    _tcsncpy(m_nid.szInfoTitle, pszTitle, _countof(m_nid.szInfoTitle));
    _tcsncpy(m_nid.szInfo, pszMsg, _countof(m_nid.szInfo));
    return Shell_NotifyIcon(NIM_MODIFY, &m_nid);
} 
#endif 

#if (_WIN32_IE  >= 0x0500) 

#ifdef _UNICODE
BOOL CTrayIcon::GetBalloonTip(std::wstring& strTitle, std::wstring& strMsg, UINT& nTimeout) const 
{ 
    strTitle = m_nid.szInfoTitle; 
    strMsg	 = m_nid.szInfo; 
    nTimeout = m_nid.uTimeout; 
    return TRUE; 
}
#else
BOOL CTrayIcon::GetBalloonTip(std::string& strTitle, std::string& strMsg, UINT& nTimeout) const 
{ 
    strTitle = m_nid.szInfoTitle; 
    strMsg	 = m_nid.szInfo; 
    nTimeout = m_nid.uTimeout; 
    return TRUE; 
}
#endif

#endif 

BOOL CTrayIcon::ShowTray(BOOL bShow)
{
    return Shell_NotifyIcon(bShow  ? NIM_ADD : NIM_DELETE,  &m_nid);
}