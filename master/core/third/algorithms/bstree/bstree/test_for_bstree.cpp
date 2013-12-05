/*******************************************************************************
*
* FileName : test_for_bstree.cpp
* Comment  : test code for binary search tree
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-9-23 9:53
*
*******************************************************************************/

#include <iostream>
#include "bstree.h"

int main()
{
	Bstree<int> bstree;
	bstree.insert(5);
	bstree.insert(3);
	bstree.insert(7);
	bstree.insert(4);
	bstree.print(std::cout);
	bstree.erase(5);
	bstree.erase(10);
	bstree.print(std::cout);
	return 0;
}