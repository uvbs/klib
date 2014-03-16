
#ifndef _UTIL_STACKTRACE_HPP_
#define _UTIL_STACKTRACE_HPP_

#include <string>

namespace util {

    std::string getStackTrace(bool writeVariables = false);

} // namespace util

#endif // !_UTIL_STACKTRACE_HPP_
