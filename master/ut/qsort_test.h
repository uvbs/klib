#pragma once



#include <algo/qsort.h>


void functor(int& x)
{
    std::uniform_int_distribution<> dist(1, 500);
    std::mt19937 gen;
    x =  dist(gen);
}

TEST(qsort, 1)
{


    std::vector<int> b(200);
    std::for_each(b.begin(), b.end(), functor);

    quick_sort(b.begin(), b.end());


    char a[] = "ksdjfohs09458204jkdv9suf98445";
    quick_sort((char*)a, a + strlen(a));

}