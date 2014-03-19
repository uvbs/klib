
//*****************************************************************************
// Copyright (c) 2003. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

//*****************************
// DEPRECATED!!!
//*****************************

// Minimalistic unit testing, implemented with two macros, TEST() and CHECK(), used as follows:
//
//    void func() {
//        TEST( "Arithmetic");
//        {
//            TEST( "Addition" );
//            CHECK( 1+1==2 );
//        }
//        {
//            TEST( "Subtraction" );
//            CHECK( 2-1==1 );
//        }
//        {
//            TEST( "Multiplication" );
//            CHECK( 2*3==6 );
//        }
//        {
//            TEST( "Division" );
//            CHECK( 4/2==2 );
//        }
//    }
//
//    AUTO_RUN( func() );
//
// AUTO_RUN() causes the function to be executed, and when the program exits, test results will be printed to OutputDebugString(). 
// Failed tests are presented with line #, and the hierarchy of tests in which the failure occurred.

#ifndef _UTIL_UNITTESTING_HPP_
#define _UTIL_UNITTESTING_HPP_

#include <algorithm>
#include <iterator>
#include <sstream>
#include <vector>

#include "AutoRun.hpp"
#include "ThreadLibrary.hpp"
#include "Trace.hpp"
#include "VariableArgMacro.hpp"

#include "Platform/Platform.hpp"

inline bool fuzzy_eq(double d1, double d2, float tolerance = .1) {
    return (d1-d2)*(d1-d2) < tolerance;
}

inline bool fuzzy_eq(double *pd1, double *pd2, unsigned int len, float tolerance = .1)
{
    unsigned int i=0;
    while (i<len && (pd1[i] - pd2[i])*(pd1[i] - pd2[i]) < tolerance)
        i++;
    return i == len;
}

class UnitTesting {
public:

    friend struct UnitTestingCheckFunctor;

    // Mutexes not required when calling stack(), since it uses thread specific storage
    static void push(std::string desc) { stack().push_back( desc ); }
    static void pop() {    stack().pop_back(); }
    static void print_stack()
    {
        std::ostringstream ostr;
        ostr << "TEST: ";
        std::copy( stack().begin(), stack().end(), std::ostream_iterator<std::string>(ostr,"/") );
        ostr << "\n";
        Platform::OS::OutputDebugString( ostr.str().c_str() );
    }

    static void displayResults()
    {
        Platform::Threads::mutex::scoped_lock scoped_lock( getMutex() );

        testResults().display();
    }

    static unsigned int failedTestCount() 
    { 
        Platform::Threads::mutex::scoped_lock scoped_lock( getMutex() );

        return testResults().failedTestCount(); 
    }

    static void dismissGuard() 
    {
        Platform::Threads::mutex::scoped_lock scoped_lock( getMutex() );

        testResults().dismissGuard();
    }

    static void init()
    {
        getMutex();
    }

public:

    class UnitTestingStackEntry {
    public:
        UnitTestingStackEntry(std::string str) { UnitTesting::push( str ); UnitTesting::print_stack();}
        ~UnitTestingStackEntry() { UnitTesting::pop(); }
    };

    class UnitTestingSentryT {
    public:
        bool &init() { static bool init_ = false; return init_; }
        UnitTestingSentryT() { if (!init()) init() = true; }
        ~UnitTestingSentryT() { static bool first = true; if (first) UnitTesting::displayResults(); first = false; }
    };

    

private:

    typedef Platform::Threads::mutex Mutex;
    static Mutex &getMutex() {
        static Mutex *mutex = new Mutex;
        return *mutex;
    }
    
    class TestResults {
    public:
        TestResults() : nPassedTests_(0), guard_(true) {}
        void dismissGuard() { guard_ = false; }
        unsigned int &passedTestCount() { return nPassedTests_; }
        unsigned int failedTestCount() { return static_cast<unsigned int>(failedTests_.size()) + (guard_ ? 1 : 0) ; }
        std::vector<std::string> &failedTests() { return failedTests_; }

        void display() {

            std::ostringstream str;

            str << "******************************************\n";
            Platform::OS::OutputDebugString( str.str().c_str() );

            str.str("");
            str << "Unit testing results:\n";
            Platform::OS::OutputDebugString( str.str().c_str() );

            str.str("");
            str << "Passed tests: " << passedTestCount() << "\n";
            Platform::OS::OutputDebugString( str.str().c_str() );

            str.str("");
            str    << "Failed tests: " << failedTestCount() << "\n";
            Platform::OS::OutputDebugString( str.str().c_str() );

            for (std::vector<std::string>::iterator it = failedTests().begin(); it != failedTests().end(); it++) {
                str.str("");
                str << *it << "\n";
                Platform::OS::OutputDebugString( str.str().c_str() );
            }

            if (guard_) {
                str.str("");
                str << "TEST FAIL: Unit testing guard was not dismissed (call UnitTesting::dismissGuard()).\n";
                Platform::OS::OutputDebugString( str.str().c_str() );
            }

            str.str("");
            str << "******************************************\n";
            Platform::OS::OutputDebugString( str.str().c_str() );

        }

        TestResults(const TestResults &);
        TestResults &operator=(const TestResults &);

        unsigned int nPassedTests_;
        std::vector<std::string> failedTests_;

        bool guard_;
    };

    static TestResults &testResults() {
        static TestResults *testResults = new TestResults();
        return *testResults;
    }

    typedef std::vector<std::string> StackT;

    static StackT &stack() {
        static Platform::Threads::thread_specific_ptr<StackT>::Val stack;
        if (stack.get() == NULL)
            Platform::Library::ResetPtr(stack, new StackT);
        return *stack;
    }

};

struct UnitTestingCheckFunctor : public util::VariableArgMacroFunctor 
{
    UnitTestingCheckFunctor() : cond_(false) {}
    UnitTestingCheckFunctor &init_check(bool cond) { cond_ = cond; return *this; }
    void deinit() 
    { 
        bool failed = !cond_;
        if (failed) { 
            args << "\n"; 
            UTIL_TRACE_DEFAULT("CHECK FAIL\n" + header.str() + args.str());
            {
                std::ostringstream str;
                str << header.str() << std::endl;
                str << "Test stack: ";
                std::copy( UnitTesting::stack().begin(), UnitTesting::stack().end(), std::ostream_iterator<std::string>(str,"/") );
                str << "\n";
                UnitTesting::testResults().failedTests().push_back( str.str() );
            }
        }
        else {
            UnitTesting::testResults().passedTestCount()++;
        }
    }

private:
    bool cond_;
};


#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4355 )  // warning C4355: 'this' : used in base member initializer list
#endif

DECLARE_VARIABLE_ARG_MACRO( UTIL_CHECK, UnitTestingCheckFunctor );
#define UTIL_CHECK(cond)                    VariableArgMacro<UnitTestingCheckFunctor >().init_check( cond ? true : false ).init( cond ? "CHECK OK" : "CHECK FAIL", #cond, __FILE__, __LINE__, BOOST_CURRENT_FUNCTION ).cast( (VariableArgMacro<UnitTestingCheckFunctor> *) NULL).UTIL_CHECK_A
#define UTIL_CHECK_A(x)                     UTIL_CHECK_OP(x, B)
#define UTIL_CHECK_B(x)                     UTIL_CHECK_OP(x, A)
#define UTIL_CHECK_OP(x, next)              UTIL_CHECK_A.notify_((x), #x).UTIL_CHECK_ ## next

#ifdef _MSC_VER
#pragma warning( pop )
#endif

// Need to initialize trace channel and mutex to avoid multi-threading issues with lazy initialization
AUTO_RUN_NAMED( UnitTesting::init(), UnitTestingInit );

// Destructor will print out the unit testing results
static UnitTesting::UnitTestingSentryT UnitTestingSentry;

#define TEST(arg) UnitTesting::UnitTestingStackEntry UnitTestingStackEntryInst(arg);
#define CHECK(cond) UTIL_CHECK(cond)

#endif //! _UTIL_UNITTESTING_HPP_
