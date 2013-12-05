/*******************************************************************************
*
* FileName : Auto_ptr.cpp
* Comment  : test code for auto pointer
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-10-10 1:25
*
*******************************************************************************/

#include <iostream>
#include "Auto_ptr.h"

int main()
{
	int *a = new int(5);
	Auto_ptr<int> p(a);
	std::cout<<*p<<"\n";
	return 0;
}