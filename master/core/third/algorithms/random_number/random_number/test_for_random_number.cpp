/*******************************************************************************
*
* FileName : test_for_random_number.cpp
* Comment  : test code
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-10-9 12:44
*
*******************************************************************************/

#include <iostream>
#include <ctime>
#include "random_number.h"

int main()
{
	set_random_limit(1000);
	clock_t tbeg = clock();
	int i = 1;
	while(true) {
		int tm = random_number();
		std::cout<<"["<<i<<"]\t"<<tm<<"\t";
		++i;
		if (i>1000) break;
		tm = random_number();
		std::cout<<"["<<i<<"]\t"<<tm<<"\t";
		tm = random_number();
		++i;
		if (i>1000) break;
		std::cout<<"["<<i<<"]\t"<<tm<<"\n";
		++i;
		if (i>1000) break;
	}
	clock_t tend = clock();
	std::cout<<"______________\ntakes "<<static_cast<double>(tend-tbeg)/CLOCKS_PER_SEC
		<<" seconds\n";
	return 0;
}