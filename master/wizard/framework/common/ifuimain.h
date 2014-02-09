
#pragma once

#include "wrappercall.h"
#include <functional>


// {79FFDBC0-C326-4745-A559-EA12E4DB03DC}
static const GUID IID_UIMAIN = 
{ 0x79ffdbc0, 0xc326, 0x4745, { 0xa5, 0x59, 0xea, 0x12, 0xe4, 0xdb, 0x3, 0xdc } };


typedef std::function<void()>  SyncFuncType;

struct IUIMain {
    // 主窗体句柄 
    virtual HWND GetMainWnd() = 0;

    // 同步调用，委托调用
	virtual BOOL SyncCall(SyncFuncType& fun) = 0;

    // 控制是否显示图标
    virtual BOOL ShowIcon(BOOL bShow) = 0;

    // 设置图标
    virtual BOOL SetIcon(HICON) = 0;

    // 开始闪烁图标
    virtual BOOL StartFlashIcon() = 0;

    // 停止闪烁图标
    virtual BOOL StopFlashIcon() = 0;

    // 返回是否在闪烁图标
    virtual BOOL IsFlashIcon() = 0;

    // 设置最顶端显示
    virtual BOOL SetTopMost(BOOL bTopMost) = 0;

    // 获取是否是最顶端
    virtual BOOL GetTopMost() = 0;

    //@todo 添加其它接口
};