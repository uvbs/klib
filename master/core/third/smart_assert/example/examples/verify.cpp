// verify.cpp

/*
    Simple usage of SMART_VERIFY
*/

#include "../smart_assert/smart_assert.h"

void test_verify() {

    int i, j, k;
    k = 1;
    i = 3;
    j = 2;

    // usage - just like SMART_ASSERT !!!
    try {
        SMART_VERIFY( (i < j) || (i < 0) || (k == -1) ) (i)(j)(k);
        std::cout << "SHOULD NEVER ARRIVE HERE!!!" << std::endl;
    }
    catch ( std::exception & exc) {
        std::cout << exc.what() << std::endl;
    }

    // using the .msg function
    try {
        SMART_VERIFY( (i < j) && (i == 3) && (j == 1) ) (i)(j)
            .msg( "as expected, this failed");
        std::cout << "SHOULD NEVER ARRIVE HERE!!!" << std::endl;
    }
    catch ( std::exception & exc) {
        std::cout << exc.what() << std::endl;
    }

    // setting level to 'debug'
    int *p = 0;
    SMART_VERIFY(p).debug( "we'll ask the user what to do");

}


int main()
{
    // where should we do the logging?
    // (to file 'asserts.txt')
    Assert::set_log( "verify.txt");

    test_verify();
    return 0;
}

