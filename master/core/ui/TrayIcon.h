#pragma once

#include <shellapi.h>
#include <string>

namespace klib{
namespace ui {


class   CTrayIcon 
{ 
public: 
    CTrayIcon(); 
    ~CTrayIcon(); 
    //   Set   notify   message   window 
    BOOL   SetNotifyWnd(HWND hWnd,  UINT nMsg); 
    BOOL   SetIcon(HICON   hIcon);	//   Set   tray   icon 
    BOOL   SetIcon(UINT   nID);	 //   Set   tray   icon   by   resource 
    HICON   GetIcon()   const;	 //   Get   tray   icon   handle 

    BOOL   SetToolTip(LPCTSTR  pszTip);	//   Set   tooltip   text 
    BOOL   SetToolTip(UINT   nID);	 //   Set   tooltip   text   by   resource 
    LPCTSTR  GetToolTip()   const;	 //   Get   tooltip   text 

#if  (_WIN32_IE >= 0x0500)
    BOOL   ShowBalloonTip(LPCTSTR pszTitle, LPCTSTR  pszMsg,   UINT nTimeout=3000);

#ifdef _UNICODE
    BOOL   GetBalloonTip(std::wstring& strTitle, std::wstring& strMsg, UINT& nTimeout)   const; 
#else
    BOOL   GetBalloonTip(std::string& strTitle, std::string& strMsg, UINT& nTimeout)   const; 
#endif
#endif 

    BOOL   ShowTray(BOOL   bShow);

private: 
    NOTIFYICONDATA   m_nid; 
};


}}
