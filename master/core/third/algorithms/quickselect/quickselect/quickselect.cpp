/*******************************************************************************
*
* FileName : quickselect.cpp
* Comment  : test code for quick select
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-9-28 11:31
*
*******************************************************************************/

#include <iostream>
#include "quickselect.h"

int main()
{
	int a[] = {34,12,0,7,83,21,3,5,12,2};
	std::cout<<"the array:\n";
	for (int i=0; i!=10; ++i)
		std::cout<<a[i]<<' ';
	std::cout<<"\n---------------------------------------\n";
	quickselect(a,10,4);
	std::cout<<"the 4-th smallest element is: "<<a[3]<<"\n";
	return 0;
}