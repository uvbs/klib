/*******************************************************************************
*
* FileName : test_for_bin_queue.cpp
* Comment  : test code for binomial queue
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-9-27 0:14
*
*******************************************************************************/

#include <iostream>
#include "bin_queue.h"

int main()
{
	Bin_queue<int> bq(10);
	bq.insert(1);
	bq.insert(2);
	bq.insert(3);
	bq.insert(4);
	bq.insert(5);
	bq.insert(6);
	bq.insert(7);

	int temp = bq.erase_min();
	std::cout<<temp<<std::endl;

	temp = bq.erase_min();
	std::cout<<temp<<std::endl;
	temp = bq.erase_min();
	std::cout<<temp<<std::endl;
	temp = bq.erase_min();
	std::cout<<temp<<std::endl;
	temp = bq.erase_min();
	std::cout<<temp<<std::endl;
	temp = bq.erase_min();
	std::cout<<temp<<std::endl;
	temp = bq.erase_min();
	std::cout<<temp<<std::endl;
	temp = bq.erase_min();
	std::cout<<temp<<std::endl;
	return 0;
}