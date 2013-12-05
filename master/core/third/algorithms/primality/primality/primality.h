/******************************************************************************
*
* FileName : primality.h
* Comment  : primality testing, is_prime() is randomized algorithms, it use
*			 Fermat's Lesser Theorem: "If P is prime and 0 < A < P, then
*			 A^(P-1) = 1 (mod P)"; alse use theorem: "If p is prime and 0 < x < p, 
*			 the only solutions to x^2 = 1 (mod P) are x = 1, p-1."
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-10-9 20:56
*
*******************************************************************************/

#ifndef _PRIMALITY_H_INCLUDED
#define _PRIMALITY_H_INCLUDED

#include "../../huge_int/huge_int/huge_int.h"
#include "../../random_number/random_number/random_number.h"

//---------------huge int version----------------
Huge_int witness(Huge_int a, Huge_int i, Huge_int n)
{
	if (0 == i)
		return 0;
	Huge_int x = witness(a, i/2, n);
	if (0 == x)
		return 0;
	Huge_int y = (x * x) % n;
	if (1 == y && x!=1 && x!= n-1)
		return 0;
	if (i%2 != 0)
		y = (y * a) % n;
	return y;
}

bool is_prime(Huge_int& n)
{
	if (n <= 1) 
		return false;
	if (2 == n)
		return true;
	long a;
	while (1) {
		a = random_number();
		if (a >= 2)
			break;
	}
	return witness(a,n-1,n) == 1;
}

bool is_exact_prime(Huge_int& n)
{
	if (n <= 1) 
		return false;
	if (2 == n)
		return true;
	for (Huge_int i(2); i <= n/2; ++i) {
		if (n % i == 0)
			return false;
	}
	return true;
}

//-------------long int version------------
int witness(int a, int i, int n)
{
	if (0 == i)
		return 1;
	int x = witness(a, i/2, n);
	if (0 == x)
		return 0;
	int y = (x * x) % n;
	if (1 == y && x!=1 && x!= n-1)
		return 0;
	if (i%2 != 0)
		y = (y * a) % n;
	return y;
}

bool is_prime(int n)
{
	if (n <= 1) 
		return false;
	if (2 == n || 3 == n)
		return true;
	set_random_limit(n-2);
	int a;
	while (1) {
		a = random_number();
		if (a >= 2)
			break;
	}
	return witness(a,n-1,n) == 1;
}

bool is_exact_prime(int n)
{
	if (n <= 1) 
		return false;
	if (2 == n)
		return true;
	for (int i = 2; i <= n/2; ++i) {
		if (n % i == 0)
			return false;
	}
	return true;
}

#endif