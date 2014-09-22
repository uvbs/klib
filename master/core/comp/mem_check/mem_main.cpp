// mem_check.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include "./src/record_helper.h"

#ifdef _DEBUG

void test_func()
{
    int* p = Type(int).create();

}


#endif