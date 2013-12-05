/*******************************************************************************
*
* FileName : random_number.h
* Comment  : pseudo random number generator, you can set the upper limit and seed
*			 linear congruential generator: x(i+1) = A*x(i) mod M
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-10-9 12:43
*
*******************************************************************************/

#ifndef _RANDOM_NUMBER_H_INCLUDED
#define _RANDOM_NUMBER_H_INCLUDED

namespace {
class Random_number {
public:
	Random_number() {}

	void set_seed (long seed) { hb_seed = seed; }
	void set_upper_limit(long limit) { upper_limit = limit; }
	long random() const;

private:
	static long hb_seed;
	static const long A = 48271;
	static const long M = 2147483647;
	static long upper_limit;
};

const long Random_number:: A;
const long Random_number:: M;
long Random_number::hb_seed = 1;
long Random_number::upper_limit = Random_number:: M;

long Random_number::random() const
{
	long Q = M / A;
	long R = M % A;
	long temp;
	temp = A * (hb_seed % Q) - R * (hb_seed / Q);
	if (temp >= 0)
		hb_seed = temp;
	else
		hb_seed = temp + M;

	return hb_seed % upper_limit;
}
} // namespace

//---------------global function--------------
long random_number()
{
	Random_number rand;
	return rand.random();
}

void set_random_seed(long seed)
{
	Random_number rand;
	rand.set_seed(seed);
}

void set_random_limit(long limit)
{
	Random_number rand;
	rand.set_upper_limit(limit+1);
}

#endif