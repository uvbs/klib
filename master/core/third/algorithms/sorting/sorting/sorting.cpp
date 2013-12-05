/*******************************************************************************
*
* FileName : sorting.cpp
* Comment  : test code for some internal sorting algorithms
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-9-27 23:03
*
*******************************************************************************/

#include <iostream>
#include "sorting.h"

int main()
{
	int a[] = {3,34,2,5,30,1,11,22,4,8};
//	insertion_sort(a,10);
//	shellsort(a,10);
//	heapsort(a,10);
//	mergesort(a,10);
	quicksort(a,10);
	for (int i=0; i!=10; ++i)
		std::cout<<a[i]<<" ";
	std::cout<<'\n';
	return 0;
}