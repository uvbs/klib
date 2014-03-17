// different_levels.cpp

/*
    examples of different levels of SMART_ASSERT
    (warn, debug, error, fatal)

    also, a simple example of a custom handler
    (print_details_to_console)
*/

#include "../smart_assert/smart_assert.h"

// custom handler !!!
void print_details_to_console( const assert_context & context) {
    smart_assert::dump_context_detail( context, std::cout );
}


void test_assert_different_levels()
{
    // LEVEL warning
    int nUsersCount = 900;
    SMART_ASSERT( (nUsersCount * 10/9) < 1000) (nUsersCount)
        .warn( "Users Count almost reaching max. capacity (1000)!");


    // LEVEL debug
    int i = 3000;
    SMART_ASSERT( i < 100) (i).debug( "index too big!");


    // LEVEL error
    try {
        int i = 0, j = 0;
        SMART_ASSERT(i != j)(i)(j).error( "values should be different!");
#ifdef SMART_ASSERT_DEBUG
        std::cout << "SHOULD NEVER ARRIVE HERE" << std::endl;
#endif
    }
    catch ( std::exception & exc) {
        std::cout << exc.what() << std::endl;
    }

    
    // LEVEL critical
    //
    // note: this, by default, would abort.
    // However, we change this, to just dump to console
    Assert::set_handler(
        lvl_fatal, &print_details_to_console);
    std::string strUserName = "titi";
    SMART_ASSERT( strUserName == "admin")(strUserName)
        .fatal( "User is not an administrator!");
    // set the default handler back
    Assert::set_handler(
        lvl_fatal, &::smart_assert::default_fatal_handler);

}



int main()
{
    // where should we do the logging?
    // (to file 'asserts.txt')
    Assert::set_log( "asserts.txt");
    test_assert_different_levels();

    return 0;
}

