
#include "../MyService.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

class MyServiceImpl
{
public:
    void reverse(std::vector<std::string> &v)
    {
        std::cout << "Reversing a vector of strings...\n";
        std::vector<std::string> w;
        std::copy(v.rbegin(), v.rend(), std::back_inserter(w));
        v.swap(w);
    }
};

int main()
{
    RCF::RcfServer server(50001);
    server.bind<MyService, MyServiceImpl>();
    server.start();
    return 0;
}