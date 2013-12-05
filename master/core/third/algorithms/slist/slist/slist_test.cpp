/*******************************************************************************
*
* FileName : slist_test.cpp
* Comment  : test code for single linked list
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-9-21 20:24
*
*******************************************************************************/

#include <iostream>
#include "slist.h"

int main()
{
	Slist<int> slist;
	int i=slist.size();
	std::cout<<i<<std::endl;
	
	slist.push_back(2);
	for (int j1=0; j1<slist.size(); ++j1)
		std::cout<<slist.value(j1+1)<<'-';
	std::cout<<std::endl;
	slist.push_front(4);
	for (int j2=0; j2<slist.size(); ++j2)
		std::cout<<slist.value(j2+1)<<'-';
	std::cout<<std::endl;
	slist.insert_after(7,1);
	i=slist.size();
	std::cout<<i<<std::endl;
	for (int j3=0; j3<slist.size(); ++j3)
		std::cout<<slist.value(j3+1)<<'-';
	std::cout<<std::endl;

	slist.reverse();
	std::cout<<slist.size()<<std::endl;
	for (int j4=0; j4<slist.size(); ++j4)
		std::cout<<slist.value(j4+1)<<'-';
	std::cout<<std::endl;
	slist.clear();
	std::cout<<slist.size()<<std::endl;
	return 0;
}