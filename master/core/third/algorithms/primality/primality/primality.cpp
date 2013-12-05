/*******************************************************************************
*
* FileName : test_for_primality.cpp
* Comment  : test code
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-10-9 20:56
*
*******************************************************************************/

#include <iostream>
#include "primality.h"

int main()
{
	int n = 0;
	while (true) {
		++n;
		if (n>=100) break;
		std::cout<<n<<"\n";
		int i = is_prime(n);
		if (i)
			std::cout<<"is prime\n";
		else
			std::cout<<"not prime\n";
		i = is_exact_prime(n);
		if (i)
			std::cout<<"exactly is prime\n";
		else
			std::cout<<"exactly not prime\n";
		}
	return 0;
}