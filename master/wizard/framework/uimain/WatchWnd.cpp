#include "StdAfx.h"
#include "WatchWnd.h"


CWatchWnd::CWatchWnd(CPaintManagerUI* pPaintManager)
{
    m_pMainWndManager = pPaintManager;
}

CWatchWnd::CWatchWnd()
{
    m_pMainWndManager  = NULL;
}

CWatchWnd::~CWatchWnd(void)
{
}


LRESULT CWatchWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;
    BOOL bHandled = FALSE;
    switch( uMsg ) {
    case WM_TIMER:
        {
            OnTimer(uMsg, wParam, lParam, bHandled);
        }
        break;
    case WM_CREATE:
        {
            OnCreate(uMsg, wParam, lParam, bHandled);
        }
        break;
    default:
        bHandled = FALSE;
    }
    if( bHandled ) return lRes;

    return CWindowUI::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CWatchWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    //__super::OnCreate(uMsg, wParam, lParam, bHandled);
    //m_PaintManager->SetMaxInfo(100, 50);

    SetWindowPos(GetHWND(), HWND_TOPMOST, 0,0, 400, 30, SWP_NOMOVE|SWP_NOACTIVATE);

    SetTimer(m_hWnd, 100, 300, NULL);
    return 0;
}

LRESULT CWatchWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (wParam == 100) {
        if (NULL == m_pMainWndManager) {
            return 0;
        }

        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(m_pMainWndManager->GetPaintWindow(), &pt);

        CControlUI* pControl = m_pMainWndManager->FindControl(pt);
        if (pControl) {
            //
            CDuiString strInfo;
            strInfo += pControl->GetClass();
            strInfo += _T(" -- ");
            strInfo += pControl->GetName();
            strInfo += _T(" #### ");
            strInfo += pControl->GetText();
            SetWindowText(GetHWND(), strInfo);
        }
    }

    return 0;
}