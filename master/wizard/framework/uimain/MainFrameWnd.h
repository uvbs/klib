#pragma once

#include "UIWindow.h"
#include <UI/TrayIcon.h>
#include <core/task_mgr.h>
#include "..\common\ifuimain.h"

using namespace klib::ui;

class CUIMainModule;

class CMainFrameWnd : public DuiLib::CWindowUI
{
public:
    CMainFrameWnd(IUIMain* pUIMain);
    ~CMainFrameWnd(void);

    friend CUIMainModule;

public:
    LPCTSTR GetWindowClassName() const { return _T("CTSFrameWnd"); }
    UINT    GetClassStyle()      const { return CS_DBLCLKS; }

    void    Init();

protected:

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    //----------------------------------------------------------------------
    // Summary;:
    //      常用消息处理函数
    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnTrayIconNotify(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnDelegateCall(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnTimer(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
    // control func
    void EnableFlashIcon(BOOL bEnable);


protected:
    bool OnBtnHomePageClick( void* param );

protected:
    CTrayIcon               m_TrayIcon;
    HICON                   m_hTmpIcon;
    BOOL                    m_bFlashIcon;

    CHorizontalLayoutUI*    m_pPlugContainer;
    IUIMain*                m_pUIMain;
    
};

