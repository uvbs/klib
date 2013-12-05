/*******************************************************************************
*
* FileName : stack_test.cpp
* Comment  : test code for stack
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-9-21 21:21
*
*******************************************************************************/

#include <iostream>
#include "stack.h"

int main()
{
	Stack<int> stack;
	stack.push(2);
	stack.push(6);
	stack.push(9);
	int num=stack.size();
	for (int i=0; i!=num; ++i)
		std::cout<<stack.pop()<<"-";
	std::cout<<std::endl;
	return 0;
}