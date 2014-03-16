
//*****************************************************************************
// Copyright (c) 2003. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

#ifndef _UTIL_INTRUSIVESTACKTRACE_HPP_
#define _UTIL_INTRUSIVESTACKTRACE_HPP_

#include <vector>
#include <iterator>
#include <algorithm>

#include "Platform/Platform.hpp"

//*************************************************
// Stack trace utility

namespace util {

    class StackTrace {
    public:
        static void push(std::string func) { stack().push_back( func );    }
        static void pop() { stack().pop_back();    }
        static std::string get_stack_trace()
        {
            std::ostringstream ostr;
            std::copy( stack().begin(), stack().end(), std::ostream_iterator<std::string>(ostr,"/") );
            ostr << "\n";
            return ostr.str();
        }

    private:

        typedef std::vector<std::string> StackT;
        static StackT &stack() {
            static Platform::Threads::thread_specific_ptr<StackT>::Val stack;
            if (stack.get() == NULL)
                Platform::Library::ResetPtr(stack, new StackT);
            return *stack;
        }

    }; 

    class StackTraceEntry {
    public:
        StackTraceEntry(std::string str) { StackTrace::push( str ); }
        ~StackTraceEntry() { StackTrace::pop(); }
    };

} // namespace util

#define STACKTRACE() util::StackTraceEntry stackTraceEntry(BOOST_CURRENT_FUNCTION)
#define STACKTRACE2(sz) util::StackTraceEntry stackTraceEntry(sz)

#endif // ! _UTIL_INTRUSIVESTACKTRACE_HPP_
