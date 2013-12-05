/*******************************************************************************
*
* FileName : test_for_huge_int.cpp
* Comment  : test code for huge int
* Version  : 1.0
* author   : haibindev.cnblogs.com
* Date     : 2007-10-9 11:20
*
*******************************************************************************/

#include <iostream>
#include <ctime>
#include "huge_int.h"

int main()
{
	std::cout<<"huge int test:\n-------------------------------\n";
	Huge_int a;
	Huge_int b;
	std::cout<<"a: ";
	std::cin>>a;
	std::cout<<"b: ";
	std::cin>>b;
	clock_t tbegin = clock();
	std::cout<<"*************************result*************************\n";
	std::cout<<"a = "<<a<<"\n"<<" b = "<<b<<"\n";
	std::cout<<"a-b = "<<a-b<<"\n";
	std::cout<<"a+b = "<<a+b<<"\n";
	std::cout<<"a*b = "<<a*b<<"\n";
	std::cout<<"a/b = "<<a/b<<"\n";
	std::cout<<"a%b = "<<a%b<<"\n";
	std::cout<<"a.pow(3) = "<<a.pow(3)<<"\n";
	a++;
	std::cout<<"a++ = "<<a<<"\n";
	a--;
	std::cout<<"a-- = "<<a<<"\n";
	std::cout<<"++b = "<<++b<<"\n";	
	std::cout<<"--b = "<<--b<<"\n";
	clock_t tend = clock();
	std::cout<<"\n==> take "<<static_cast<double>(tend-tbegin)/CLOCKS_PER_SEC
		<<" seconds\n";
	return 0;
}