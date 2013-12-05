/*******************************************************************************
*
* FileName : test_for_RBtree.cpp
* Comment  : test code for red-black tree
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-10-12 23:03
*
*******************************************************************************/

#include <iostream>
#include "RBtree.h"

int main()
{
	RBtree<int> rbtree;
	std::cout<<rbtree.size()<<"\n";
	rbtree.insert(10);
	rbtree.insert(7);
	rbtree.insert(8);
	rbtree.insert(15);
	rbtree.insert(5);
	rbtree.insert(6);
	rbtree.insert(11);
	rbtree.insert(13);
	rbtree.insert(12);
	std::cout<<rbtree.size()<<"\n";
	for (RBtree<int>::iterator prb = rbtree.begin();
		prb != rbtree.end(); ++prb)
		std::cout<<*prb<<" ";
	std::cout<<"\n";
	return 0;
}