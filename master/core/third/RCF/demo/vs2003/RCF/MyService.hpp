
#ifndef _MYSERVICE_HPP_
#define _MYSERVICE_HPP_

#include <string>
#include <vector>

#include <RCF/RCF.hpp>
#include <SF/vector.hpp>

RCF_BEGIN(MyService, "MyService")
    RCF_METHOD_V1(void, reverse, std::vector<std::string> &);
RCF_END(MyService);

#endif // ! _MYSERVICE_HPP_