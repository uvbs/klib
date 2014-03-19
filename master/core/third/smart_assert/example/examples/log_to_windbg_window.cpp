// log_to_windbg_window.cpp

/*
    logs failed asserts to the Debug window 
    (Win32 only!)

    (note: in this example all asserts are ignored;
     this is because we set all handlers to ignore_assert,
     see below)

     Ignoring asserts is VERY useful for automated tests.
*/

#include "../smart_assert/smart_assert.h"
#include <windows.h>
#include <sstream>


void ignore_assert( const assert_context & ) {
    // do nothing
}


//
// outputs using OutputDebugString
//
void output_to_debug_wnd_win32( const assert_context & context) {

    // follow VC convension:
    // first, write "File(Line) : ...", then on the next lines
    // write the rest of the message;
    //
    // this way, someone can actually double click on the first line,
    // and see where the assertion occured (in debug mode)

    // log context
    std::ostringstream out;
    out << '\n' 
        << context.get_context_file()
        << '(' << context.get_context_line() << ") : [SMART_ASSERT]\n"
        << smart_assert::get_typeof_level( context.get_level() ) << " (details below)\n";

    // log expression
    if ( !context.get_level_msg().empty())
        out << "\n\tUser-friendly msg: '" << context.get_level_msg() << "'";
    out << "\n\tExpression = '" << context.get_expr() << "'\n";

    // log values
    out << "\n\tValues:\n";
    assert_context::vals_array::const_iterator
        first = context.get_vals_array().begin(), 
        last = context.get_vals_array().end();
    while ( first != last) {
        out << '\t' << first->second << " = " << first->first << '\n';
        ++first;
    }
    out << "---------------------------\n";
    OutputDebugString( out.str().c_str());
};

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

    std::string strUserName = "titi";
    SMART_ASSERT( strUserName == "admin")(strUserName)
        .fatal( "User is not an administrator!");

}



int main()
{
    // log to debug window
    Assert::set_log( &output_to_debug_wnd_win32);

    // Handlers: ignore all asserts
    // (they are logged though)
    Assert::set_handler( lvl_warn, &ignore_assert);
    Assert::set_handler( lvl_debug, &ignore_assert);
    Assert::set_handler( lvl_error, &ignore_assert);
    Assert::set_handler( lvl_fatal, &ignore_assert);

    test_null_str();
    test_assert();
    return 0;
}


