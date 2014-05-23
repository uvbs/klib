// fiber_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <comp/fiber/scheduler.h>
#include <comp/fiber/coroutine.h>
using namespace fiber;

//typedef coroutine_basic<>;

void func()
{

}

int _tmain(int argc, _TCHAR* argv[])
{
    scheduler_basic sche;
    sche.spawn(func);

    sche.run();

	return 0;
}

