/*******************************************************************************
*
* FileName : test_for_leftist_heap.cpp
* Comment  : test code for leftist heap
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-9-26 21:09
*
*******************************************************************************/

#include <iostream>
#include "leftist_heap.h"

int main()
{
	Left_heap<int> lheap;
	lheap.insert(26);
	lheap.insert(17);
	lheap.insert(8);
	lheap.insert(3);
	std::cout<<"leftist heap 1: \n";
	lheap.print_tree(std::cout);

	Left_heap<int> lheap_other;
	lheap_other.insert(10);
	lheap_other.insert(21);
	lheap_other.insert(14);
	lheap_other.insert(23);
	std::cout<<"leftist heap 2: \n";
	lheap_other.print_tree(std::cout);

	lheap.merge(lheap_other);
	std::cout<<"leftist heap 1 merge heap 2 : \n";
	lheap.print_tree(std::cout);
	return 0;
}