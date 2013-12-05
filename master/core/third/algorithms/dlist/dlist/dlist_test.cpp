/*******************************************************************************
*
* FileName : dlist_test.cpp
* Comment  : test code for doubly linked list
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-9-21 20:17
*
*******************************************************************************/

#include <iostream>
#include "dlist.h"

int main()
{
	Dlist<int> dlist;
	int i=dlist.size();
	std::cout<<i<<std::endl;
	
	dlist.push_back(2);
	for (int j1=0; j1<dlist.size(); ++j1)
		std::cout<<dlist.value(j1+1)<<'-';
	std::cout<<std::endl;
	dlist.push_front(4);
	for (int j2=0; j2<dlist.size(); ++j2)
		std::cout<<dlist.value(j2+1)<<'-';
	std::cout<<std::endl;
	dlist.insert_after(7,1);
	i=dlist.size();
	std::cout<<i<<std::endl;
	for (int j3=0; j3<dlist.size(); ++j3)
		std::cout<<dlist.value(j3+1)<<'-';
	std::cout<<std::endl;

	dlist.reverse();
	std::cout<<dlist.size()<<std::endl;
	for (int j4=0; j4<dlist.size(); ++j4)
		std::cout<<dlist.value(j4+1)<<'-';
	std::cout<<std::endl;
	dlist.clear();
	std::cout<<dlist.size()<<std::endl;
	return 0;
}