#pragma once


namespace DuiLib {


/// 一个处理了基本事件的窗口类，可以作为MainFrame/Dialog的基类。
/**
	CWindowWnd类创建了窗口，定义了消息流程，但对于一些基本处理没有响应。
	一个窗口如果从这里继承，会写很多代码；一个项目里多个窗口，就有很多重复的代码。
	所以写了这个类，以减少项目的工作量。如果需要更灵活、更独特的窗口，仍然可以从CWindowWnd继承。

	本窗口有如下特殊约定：
	sysbtn_min:最小化按钮
	sysbtn_max:最大化按钮
	sysbtn_cls:关闭按钮
	sysbtn_rst:恢复按钮
	sysbtn_ok:对话框的确定
	sysbtn_cancel:对话框的取消
*/

class  CWindowUI : public CWindowWnd, public INotifyUI
{
public:
	/// 构造函数
	CWindowUI(void);
	/// 析构函数
	~CWindowUI(void);

	virtual LPCTSTR GetWindowClassName() const {return _T("WindowWnd");}

	/// 通过XML创建窗口
	HWND CreateX(HWND hwndParent, DWORD dwStyle, LPCTSTR lpszXml);
	UINT DoModal(HWND hwndParent, LPCTSTR lpszXml);

	/// 处理通用消息
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	/// 处理通知消息
	void Notify(TNotifyUI& msg);

	//以下为常用消息
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);        //为子类预留接口
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);         //为子类预留接口
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);       //为子类预留接口
	virtual LRESULT OnNcDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);     //默认处理
	virtual LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);    //默认处理
	virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);     //默认处理
	virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);          //默认处理
	virtual LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled); //默认处理
	virtual LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);       //默认处理
	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);    //默认处理

public:
	/// 绘制管理器
	CPaintManagerUI m_PaintManager;

protected:
	/// 根据xml文件，创建自定义窗口
	CDuiString      m_xmlCreate;
};


} // namespace DuiLib
