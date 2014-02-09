#include "StdAfx.h"
#include "UIMainModule.h"

#include <aom/ifdef.h>
#include "helpers.h"
#include "..\common\ifchatdef.h"
#include "..\common\wndmsg.h"


#ifdef _DEBUG
#include "WatchWnd.h"
#endif

CUIMainModule::CUIMainModule(void) : m_bTopMost(TRUE)
{
    m_pMainFrameWnd = NULL;
}

CUIMainModule::~CUIMainModule(void)
{
}

void CUIMainModule::on_pre_run()
{
    klib::aom::iframework* frame_ = frame();
    
    IPlug* pIPlug = NULL;
    imodule* pModule = frame_->find_next_interface(IID_IPlug, (void**)&pIPlug, NULL);
    while (pModule) 
    {
        CPlugList::instance()->push_item(pIPlug);
        pModule = frame_->find_next_interface(IID_IPlug, (void**)&pIPlug, pModule);
    }
}

void CUIMainModule::on_post_run()
{
    CPaintManagerUI::SetInstance(GetModuleHandle(NULL));
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skins"));

    HRESULT Hr = ::CoInitialize(NULL);
    if( FAILED(Hr) ) return ;

    m_pMainFrameWnd = new CMainFrameWnd(this);
    if(m_pMainFrameWnd == NULL ) return ;
    //pFrame->Create(NULL, _T("这是一个最简单的测试用exe，修改test1.xml就可以看到效果"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
    m_pMainFrameWnd->CreateX(NULL, UI_WNDSTYLE_FRAME, _T("mainframe.xml"));
    m_pMainFrameWnd->CenterWindow();
    m_pMainFrameWnd->ShowWindow(true);

#ifdef _DEBUG
    const TCHAR* strWatchXml = _T("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\" ?>")
        _T("<Window size=\"100,50\" sizebox=\"4,4,6,6\" caption=\"0,0,0,29\" mininfo=\"100,50\" roundcorner=\"2,2\" >")
            _T("<Font name=\"宋体\" size=\"12\" bold=\"true\" italic=\"false\" />")
            _T("<VerticalLayout bordersize=\"1\" bkcolor=\"#FFFFFFFF\" bordercolor=\"#FF1B6212\">")
                _T("<HorizontalLayout  height=\"27\" >")
                    _T("<Edit name=\"wndInfo\"/>")
                _T("</HorizontalLayout>")
            _T("</VerticalLayout>")
        _T("</Window>");

    CWatchWnd* pWatchWnd = new CWatchWnd(&m_pMainFrameWnd->m_PaintManager);
    if (NULL == pWatchWnd) {
        return;
    }
    //pWatchWnd->CreateX(NULL, UI_WNDSTYLE_FRAME, _T("watchwnd.xml"));
    pWatchWnd->CreateX(NULL, UI_WNDSTYLE_FRAME, _T("mainframe.xml"));
    pWatchWnd->ShowWindow(true);
#endif
    CPaintManagerUI::MessageLoop();

    ::CoUninitialize();
}

HWND CUIMainModule::GetMainWnd() 
{
    if (NULL != m_pMainFrameWnd) {
        return m_pMainFrameWnd->GetHWND();
    }
    return NULL;
}

BOOL CUIMainModule::SyncCall(SyncFuncType& fun)
{
    ::SendMessage(m_pMainFrameWnd->GetHWND(), UM_DELEGATE_CALL, (WPARAM)&fun, 0);

    return TRUE;
}

BOOL CUIMainModule::ShowIcon(BOOL bShow) 
{
    SyncFuncType fun = [&]()
    {
        m_pMainFrameWnd->m_TrayIcon.ShowTray(bShow);
    };

    SyncCall(fun);
    return TRUE;
}

BOOL CUIMainModule::SetIcon(HICON hIcon)
{
    if (NULL == hIcon) {
        return FALSE;
    }

    SyncFuncType fun = [&]()
    {
        m_pMainFrameWnd->m_TrayIcon.SetIcon(hIcon);
    };
    SyncCall(fun);

    return TRUE;
}

BOOL CUIMainModule::StartFlashIcon()
{
    SyncFuncType fun = [&]()
    {
        m_pMainFrameWnd->EnableFlashIcon(TRUE);
    };

    SyncCall(fun);
    return TRUE;
}

BOOL CUIMainModule::StopFlashIcon()
{
    SyncFuncType fun = [&]()
    {
        m_pMainFrameWnd->EnableFlashIcon(FALSE);
    };

    SyncCall(fun);
    return TRUE;
}

BOOL CUIMainModule::IsFlashIcon()
{
    return m_pMainFrameWnd->m_bFlashIcon;
}

BOOL CUIMainModule::SetTopMost(BOOL bTopMost)
{
    HWND hWnd = m_pMainFrameWnd->GetHWND();
    HWND hWndPos = HWND_NOTOPMOST;
    m_bTopMost = FALSE;

    if (bTopMost) 
    {
        hWndPos = HWND_TOPMOST;
        m_bTopMost = TRUE;
    }

    SetWindowPos(hWnd, hWndPos, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);

    return TRUE;
}

BOOL CUIMainModule::GetTopMost() 
{
    return m_bTopMost;
}