/*******************************************************************************
*
* FileName : min_heap.cpp
* Comment  : test code for minimal heap
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-9-25 10:07
*
*******************************************************************************/

#include <iostream>
#include <limits>
#include "min_heap.h"

int main()
{
	int limit = std::numeric_limits<int>::min();
	Min_heap<int> min_heap(27, limit);
	min_heap.insert(13);
	min_heap.insert(21);
	min_heap.insert(16);
	min_heap.insert(24);
	min_heap.insert(31);
	min_heap.insert(19);
	min_heap.insert(68);
	min_heap.insert(65);
	min_heap.insert(26);
	min_heap.insert(32);
	min_heap.print(std::cout);
	std::cout<<"insert \"14\"\n";
	min_heap.insert(14);
	min_heap.print(std::cout);
	std::cout<<"erase minimal element\n";
	min_heap.erase_min();
	min_heap.print(std::cout);
	std::cout<<"decrease_key(5,20) : \n";
	min_heap.decrease_key(5,20);
	min_heap.print(std::cout);
	std::cout<<"increase_key(1,20) : \n";
	min_heap.increase_key(1,20);
	min_heap.print(std::cout);
	return 0;
}