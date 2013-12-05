// small_string.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <string>
#include <iostream>
#include <algorithm>
#include "small_string.hpp"

#include <wchar.h>

class test_reporter
{
public:
	test_reporter(char const * name)
	{
		std::cout << "begin test " << name << "\n{" << std::endl;
	}
	~test_reporter()
	{
		std::cout << "}\n" << std::endl;
	}
};

#define OUT(expr) std::cout << '\t' << #expr << std::endl; expr
#define OUT_VAL(expr) std::cout << '\t' << #expr << '=' << expr << std::endl

void test_assign()
{
	test_reporter reporter("assign");
	OUT(cfc::small_string<16> s1= "hello";)
	OUT(cfc::small_string<8>  s2= s1;)
	OUT_VAL(s1);
	OUT_VAL(s2);
	OUT(std::string s3 = s1);
	OUT_VAL(s3);
	OUT(s1=s3);
	OUT_VAL(s1);
}

void test_append()
{
	test_reporter reporter("append");
	OUT(cfc::small_string<16> s="hello";)
	OUT(s+=s;)
	OUT_VAL(s);
	OUT(s+="chen3feng");
	OUT_VAL(s);
	OUT(s="chen3feng";)
	OUT(cfc::small_string<16> s1="is me. :)";)
	OUT_VAL(s+s1);
}

void test_erase()
{
	test_reporter reporter("erase");
	OUT(cfc::small_string<16> s="hello";)
	OUT( s.erase(3);)
	OUT_VAL(s);
	OUT( s.erase(); )
	OUT_VAL(s);
}

void test_compare()
{
	test_reporter reporter("compare");
	OUT( cfc::small_string<16> s1="chen3feng"; )
	OUT( cfc::small_string<16> s2="chen4feng"; )  //查无此人,呵呵
	OUT_VAL( (s1 ==s2) );
	OUT_VAL( (s1 !=s2) );
	OUT_VAL( (s1 > s2) );
	OUT_VAL( (s1 >=s2) );
	OUT_VAL( (s1 < s2) );
	OUT_VAL( (s1 <=s2) );
	OUT( s2=s1; )
	OUT_VAL( (s1 ==s2) );
	OUT_VAL( (s1 !=s2) );
}

void test_insert()
{
	test_reporter reporter("insert");
	OUT(cfc::small_string<16> s="hello";)
	OUT( s.insert(3,"hello");)
	OUT_VAL(s);
	OUT( s.insert(3,s); )
	OUT_VAL(s);
}

static void inc(char& ch)
{
	ch++;
}

void test_iterator()
{
	test_reporter reporter("iterator");
	OUT( cfc::small_string<32>  s="abcdefghijklmnopqrstuvwxyz"; )
	OUT( std::for_each(s.begin(), s.end(), inc); )
	OUT_VAL(s);
	OUT( char * t = "12345678"; )
	OUT( s.insert(s.begin(), t ,t+8); )
	OUT_VAL(s);
}

void foo(char const *){}

#include<new>

int main(int argc, char* argv[])
{
	
	cfc::small_string<20> str;

	str = "ssssssssssssssssssssssssssssssssssssssssssssssssssssss";

	/*
	test_assign();
	test_append();
	test_erase();
	test_insert();
	//test_iterator();
	//test_compare();
*/

	std::cout<< str << std::endl;


	return 0;
}
