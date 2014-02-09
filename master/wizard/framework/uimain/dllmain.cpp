// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#include "UIMainModule.h"

#ifdef _DEBUG
#pragma comment(lib, "xlib_d.lib")
#else
#pragma comment(lib, "xlib.lib")
#endif


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
    REGISTER_MODULE(CUIMainModule)
END_REGISTER_MODULES