#include "StdAfx.h"
#include "MainFrameWnd.h"

#include "..\ts\Resource.h"
#include "..\common\ifchatdef.h"
#include "..\common\wrappercall.h"
#include "..\common\wndmsg.h"

#include "helpers.h"

#ifdef _DEBUG
#include "WatchWnd.h"
#endif



#define  WM_TRAY_NOTIFICATION  (WM_USER + 10)

// 定时器
#define  TIMER_ID_FLASH_ICON   (1001)


CMainFrameWnd::CMainFrameWnd(IUIMain* pUIMain) : 
    m_pUIMain(pUIMain),
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
            m_TrayIcon.ShowTray(FALSE);
            PostQuitMessage(0);
            break;
        }
    case WM_TIMER:
        lRes = OnTimer(wParam, lParam, bHandled);  break;
    case UM_DELEGATE_CALL:  lRes = OnDelegateCall(wParam, lParam, bHandled); break;
    default:
        bHandled = FALSE;
    }
    if( bHandled ) return lRes;

    return CWindowUI::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CMainFrameWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    __super::OnCreate(uMsg, wParam, lParam, bHandled);

    SetIcon(IDI_SMALL);

    m_TrayIcon.SetNotifyWnd(GetHWND(), WM_TRAY_NOTIFICATION);
    //m_TrayIcon.SetIcon((HICON)::LoadImage(CPaintManagerUI::instance(), MAKEINTRESOURCE(IDI_DUILIBTEST), IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR), _T("Duilib测试程序"));
    m_TrayIcon.SetIcon(IDI_SMALL);
    m_TrayIcon.SetToolTip(TS_APP_NAME);
    m_TrayIcon.ShowTray(TRUE);

    SetWindowPos(GetHWND(), HWND_TOPMOST, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);

    SetTimer(GetHWND(), TIMER_ID_FLASH_ICON, 800, NULL);

    Init();
    bHandled = TRUE;

    return 0;
}

void CMainFrameWnd::Init()
{
    CControlUI* pControl = m_PaintManager.FindControl(_T("homepage"));
    if (pControl) {
        pControl->OnNotify += MakeDelegate(this, &CMainFrameWnd::OnBtnHomePageClick);
    }

    m_pPlugContainer = (CHorizontalLayoutUI*) m_PaintManager.FindControl(_T("plugContainer"));

    //----------------------------------------------------------------------
    // 创建插件的界面
    //----------------------------------------------------------------------
    auto_lock locker(CPlugList::instance()->get_lock());

    CPlugList::instance()->for_each([&](IPlug* pPlug)->bool
    {
        pControl = pPlug->CreateUI(&m_PaintManager, m_pUIMain);
        m_pPlugContainer->Add(pControl);

        return true;
    });

    CTaskMgr::instance()->start();
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

    case WM_RBUTTONUP: // 弹出菜单
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

LRESULT CMainFrameWnd::OnDelegateCall(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    KLIB_ASSERT(wParam);
    if (wParam == 0) {
        return 0;
    }

    bHandled = TRUE;
    SyncFuncType* fun = (SyncFuncType*) wParam;
    (*fun)();

    return 0;
}

LRESULT CMainFrameWnd::OnTimer(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = TRUE;

    if (wParam == TIMER_ID_FLASH_ICON) 
    {
        static BOOL bShowIcon = FALSE;
        if (m_bFlashIcon) 
        {
            if (bShowIcon) 
            {
                m_TrayIcon.SetIcon(m_hTmpIcon);
            }
            else
            {
                m_TrayIcon.SetIcon((HICON)1);
            }

            bShowIcon = !bShowIcon;
        }
    }

    return 0;
}

void CMainFrameWnd::EnableFlashIcon(BOOL bEnable)
{
    m_bFlashIcon = bEnable;
    if (bEnable) 
    {
        m_hTmpIcon = m_TrayIcon.GetIcon();
    }
    else
    {
        if (m_hTmpIcon && m_TrayIcon.GetIcon() == NULL) 
        {
            m_TrayIcon.SetIcon(m_hTmpIcon);
        }
    }

    m_TrayIcon.ShowTray(TRUE);
}

bool CMainFrameWnd::OnBtnHomePageClick( void* param )
{
    TNotifyUI* pMsg = (TNotifyUI*) param;
    if (pMsg->sType == _T("click"))
    {
        klib::core::task_mgr::task_func_type fun = [&]()
        {
            ShellExecute(NULL, _T("open"), _T("http://www.baidu.com/"), _T(""), NULL, SW_SHOWNORMAL);
        };
        
        CTaskMgr::instance()->add_task(fun);
    }

    

    return true;
}