// ts.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "ts.h"


#include <third/duilib/UIlib.h>
#include <aom/iframework_imp.h>
#include <core/mini_dumper.h>
using namespace DuiLib;


#ifdef _DEBUG
#pragma comment(lib, "xlib_d.lib")
#else
#pragma comment(lib, "xlib.lib")
#endif


klib::debug::mini_dumper g_mini_dumper(true);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	//提示交互界面首先创建
    klib::aom::iframework* framework_ = klib::aom::framework::instance() ;

    CMarkup xml;
    bool bLoadResult = xml.LoadFromFile(_T("modules.xml"));
    if (bLoadResult)
    {
        CMarkupNode rootNode = xml.GetRoot();
        CMarkupNode modulesNode = rootNode.GetChild(_T("modules"));
        HRESULT hr = S_FALSE;
        if (modulesNode.IsValid())
        {
            CMarkupNode node = modulesNode.GetChild(_T("module"));
            while (node.IsValid())
            {
                hr = framework_->load_module(node.GetAttributeValue(_T("path")));
                if (S_FALSE == hr)
                {
                    KLIB_ASSERT(FALSE && node.GetAttributeValue(_T("path")));
                }
                node = node.GetSibling();
            }
        }
    }

    framework_->start();
    framework_->stop();
    //Sleep(-1);

	return 0;
}

