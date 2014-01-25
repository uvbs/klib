#include "StdAfx.h"
#include "MainFrameWnd.h"


#define  WM_TRAY_NOTIFICATION  (WM_USER + 10)


CMainFrameWnd::CMainFrameWnd() : 
    m_bFlashIcon(FALSE),
    m_hTmpIcon(NULL)
{
}

CMainFrameWnd::~CMainFrameWnd(void)
{
}

LRESULT CMainFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;
    BOOL bHandled = FALSE;
    switch( uMsg ) {
    case WM_CREATE:				lRes = OnCreate(uMsg, wParam, lParam, bHandled);			break;
    case WM_TRAY_NOTIFICATION:	lRes = OnTrayIconNotify(wParam, lParam, bHandled); bHandled = TRUE;	break;
    case WM_CLOSE:
        {
            PostQuitMessage(0);
            break;
        }
    case WM_TIMER:
        lRes = OnTimer(wParam, lParam, bHandled);  break;
    default:
        bHandled = FALSE;
    }
    if( bHandled ) return lRes;

    return CWindowUI::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CMainFrameWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    __super::OnCreate(uMsg, wParam, lParam, bHandled);


//     m_TrayIcon.SetNotifyWnd(GetHWND(), WM_TRAY_NOTIFICATION);
//     //m_TrayIcon.SetIcon((HICON)::LoadImage(CPaintManagerUI::instance(), MAKEINTRESOURCE(IDI_DUILIBTEST), IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR), _T("Duilib²âÊÔ³ÌÐò"));
//     m_TrayIcon.SetIcon(IDI_SMALL);
//     m_TrayIcon.SetToolTip(TS_APP_NAME);
//     m_TrayIcon.ShowTray(TRUE);

    SetWindowPos(GetHWND(), HWND_TOPMOST, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);

    Init();
    bHandled = TRUE;

    return 0;
}

void CMainFrameWnd::Init()
{

}

LRESULT CMainFrameWnd::OnTrayIconNotify(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    UINT uMouseMsg = (UINT)lParam; // mouse message that was sent.

    switch(uMouseMsg)
    {
    case WM_LBUTTONUP:
        if (::IsIconic(m_hWnd))
        {
            ::ShowWindow(m_hWnd, SW_RESTORE);
        }
        ::ShowWindow(m_hWnd, SW_SHOWNORMAL);
        ::SetForegroundWindow(m_hWnd);

        break;

    case WM_RBUTTONUP: // µ¯³ö²Ëµ¥
        {
            // 			DuiLib::CPoint pos;
            // 			::GetCursorPos(&pos);
            // 
            // 			HMENU hMainMenu = ::LoadMenu(m_PaintManager.GetInstance(), MAKEINTRESOURCE(IDC_MEDTOOLS));
            // 			HMENU hTrayMenu = GetSubMenu(hMainMenu, 2);
            // 			if (hTrayMenu)
            // 			{
            // 				::SetForegroundWindow(m_hWnd);
            // 				::TrackPopupMenu(hTrayMenu, TPM_RIGHTBUTTON|TPM_RIGHTALIGN, pos.x, pos.y, 0, m_hWnd, NULL);
            // 			}
            // 
            // 			::PostMessage(m_hWnd,WM_NULL, 0, 0);
        }
        break;

    case WM_LBUTTONDBLCLK:
        {
#ifdef _DEBUG
            PostMessage(WM_CLOSE);
#endif
        }
    }

    return 0;
}

LRESULT CMainFrameWnd::OnTimer(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = TRUE;


    return 0;
}
