// simple.cpp

/*
    simple examples of using SMART_ASSERT
*/


#include "../smart_assert/smart_assert.h"


void test_first_second( int first, int second) {
    if ( first < second)
        SMART_ASSERT( first > second)(first)(second);
    else
        SMART_ASSERT( first < second)(first)(second);
}

void test_null_str() {
    char* aline = 0;
    SMART_ASSERT( aline);
    SMART_ASSERT( aline)( aline);

    const char * aline_const = 0;
    SMART_ASSERT( aline_const);
    SMART_ASSERT( aline_const)( aline_const);
}

void test_assert()
{
    int i, j, k;
    k = 1;
    i = 3;
    j = 2;
    SMART_ASSERT( (i < j) || (i == 3) || (j == 1) ) (i)(j);
    SMART_ASSERT( (i < j) || (i < 0) || (k == -1) ) (i)(j)(k);

    // shortcut, instead of SMART_ASSERT(p)(p);
    int *p = 0;
    SMART_ASSERT(p);

    test_first_second( 1, 2);
    test_first_second( 2, 1);
}



int main(int argc, char* argv[])
{
    // where should we do the logging?
    // (to file 'asserts.txt')
    Assert::set_log( "asserts.txt");

    test_null_str();
    test_assert();
    return 0;
}

