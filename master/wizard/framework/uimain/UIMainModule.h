#pragma once

#include "ifuimain.h"

#include "MainFrameWnd.h"
#include "core/singleton.h"


class CUIMainModule:
    public klib::aom::module_imp,
    public IUIMain
{
public:
    CUIMainModule(void);
    ~CUIMainModule(void);


    BEGIN_INTERFACE_ENTRY
        INTERFACE_ENTRY(IID_UIMAIN, IUIMain)
    END_INTERFACE_ENTRY

public:
    virtual void on_pre_run();
    virtual void on_post_run();

public:
    //----------------------------------------------------------------------
    // For IUIMain
    virtual HWND GetMainWnd() ;
    virtual BOOL SyncCall(SyncFuncType& fun);
    virtual BOOL ShowIcon(BOOL bShow) ;
    virtual BOOL SetIcon(HICON);
    virtual BOOL StartFlashIcon();
    virtual BOOL StopFlashIcon();
    virtual BOOL IsFlashIcon();
    virtual BOOL SetTopMost(BOOL bTopMost);
    virtual BOOL GetTopMost() ;

    //----------------------------------------------------------------------

protected:
    CMainFrameWnd*  m_pMainFrameWnd;
    BOOL            m_bTopMost;
};

