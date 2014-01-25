<?php

function create_guid() 
{
	$charid = strtoupper(md5(uniqid(mt_rand(), true)));
	$hyphen = chr(45);// "-"
	$uuid = chr(123)// "{"
	.substr($charid, 0, 8).$hyphen
	.substr($charid, 8, 4).$hyphen
	.substr($charid,12, 4).$hyphen
	.substr($charid,16, 4).$hyphen
	.substr($charid,20,12)
	.chr(125);// "}"
	return $uuid;
}

 // 原目录，复制到的目录
function recurse_copy($src,$dst) { 

    $dir = opendir($src);
    @mkdir($dst);
    while(false !== ( $file = readdir($dir)) ) {
        if (( $file != '.' ) && ( $file != '..' )) {
            if ( is_dir($src . '/' . $file) ) {
                recurse_copy($src . '/' . $file,$dst . '/' . $file);
            }
            else {
                copy($src . '/' . $file,$dst . '/' . $file);
            }
        }
    }
    closedir($dir);
}

// 生成cpp内容
function generate_module_hpp($module_name)
{
	$module_name_template = "{<Module_Name>}";

	$template_contents =<<<EOF
#pragma once
#include "stdafx.h"

#include <map>
#include <set>
#include <string>
#include <core/lock_stl.h>
#include <kthread/auto_lock.h>
#include "MainFrameWnd.h"

using namespace klib::stl;

class {<Module_Name>} :
    public klib::aom::module_imp
{
public:
    {<Module_Name>}(void);
    virtual ~{<Module_Name>}(void);

    BEGIN_INTERFACE_ENTRY
        //INTERFACE_ENTRY(IID_IPlug, IPlug)
    END_INTERFACE_ENTRY

protected:
    virtual void on_pre_run();
    virtual BOOL on_run();
    virtual void on_post_run();
    virtual void on_post_stop();

private:
	CMainFrameWnd* m_pMainFrameWnd;
};
	
EOF;

	$template_contents = str_replace($module_name_template, $module_name, $template_contents);


	return $template_contents;
}

// 生成header内容
function generate_module_cpp($module_name)
{
	$module_name_template = "{<Module_Name>}";

	$template_contents =<<<EOF
#include "stdafx.h"

#include "{<Module_Name>}.h"

{<Module_Name>}::{<Module_Name>}(void) 
{
}

{<Module_Name>}::~{<Module_Name>}(void)
{
}

void {<Module_Name>}::on_pre_run()
{
}

BOOL {<Module_Name>}::on_run()
{
	return TRUE;
}

void {<Module_Name>}::on_post_run()
{
	CPaintManagerUI::SetInstance(GetModuleHandle(NULL));
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skins"));

    HRESULT Hr = ::CoInitialize(NULL);
    if( FAILED(Hr) ) return ;
    
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
#endif

    m_pMainFrameWnd = new CMainFrameWnd();
    if(m_pMainFrameWnd == NULL ) return ;
    //pFrame->Create(NULL, _T("这是一个最简单的测试用exe，修改test1.xml就可以看到效果"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
    //m_pMainFrameWnd->CreateX(NULL, UI_WNDSTYLE_FRAME, _T("mainframe.xml"));
    m_pMainFrameWnd->CreateX(NULL, UI_WNDSTYLE_FRAME, _T("mainframe.xml"));
    m_pMainFrameWnd->CenterWindow();
    m_pMainFrameWnd->ShowWindow(true);

    CPaintManagerUI::MessageLoop();

    ::CoUninitialize();
}

void {<Module_Name>}::on_post_stop()
{
}

EOF;

	$template_contents = str_replace($module_name_template, $module_name, $template_contents);
	
	return $template_contents;
}

function generate_dllmain($module_name)
{
	$module_name_template = "{<Module_Name>}";
	
	$template_contents =<<<EOF
// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "{<Module_Name>}.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

BEGIN_REGISTER_MODULES
    REGISTER_MODULE({<Module_Name>})
END_REGISTER_MODULES	
EOF;


	$template_contents = str_replace($module_name_template, $module_name, $template_contents);
	
	return $template_contents;
}

function generate_stdafx_cpp()
{
	$template_contents =<<<EOF
// stdafx.cpp : 只包括标准包含文件的源文件
// chat_base.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用
	
EOF;

	return $template_contents;
}

function generate_stdafx_hpp()
{
	$template_contents =<<<EOF
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>
#include <WinSock2.h>


// TODO: 在此处引用程序需要的其他头文件

#define WIN32_LEAN_AND_MEAN	
#define _CRT_SECURE_NO_DEPRECATE

#include <objbase.h>

#include <aom/iframework.h>
#include <third/duiLib/UIlib.h>

using namespace DuiLib;

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_ud.lib")
#   else
#       pragma comment(lib, "DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_u.lib")
#   else
#       pragma comment(lib, "DuiLib.lib")
#   endif
#endif
EOF;

	return $template_contents;
}
















