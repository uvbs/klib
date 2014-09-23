// mem_stats.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <comp/mem_check/library/src/mem_lib.h>
#include <comp/mem_check/library/src/object_creator.h>

#ifdef _DEBUG
#pragma comment(lib, "../../core/lib/Debug/mem_check_library.lib")
#else
#pragma comment(lib, "../../core/lib/Release/mem_check_library.lib")
#endif


int _tmain(int argc, _TCHAR* argv[])
{
    set_memory_desc("test-exe");

    int* p = Type(int).create();
    *p = 100;

    new int ;
    //delete p;

    p  = New(int);

	return 0;
}

