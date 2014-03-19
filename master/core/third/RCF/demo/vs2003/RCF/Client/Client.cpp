
#include "../MyService.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

int main()
{
    try
    {
        std::vector<std::string> v;
        v.push_back("one");
        v.push_back("two");
        v.push_back("three");

        std::cout << "Before:\n";
        std::copy(
            v.begin(), 
            v.end(), 
            std::ostream_iterator<std::string>(std::cout, "\n"));

        RcfClient<MyService>("localhost", 50001).reverse(v);

        std::cout << "\nAfter:\n";
        std::copy(
            v.begin(), 
            v.end(), 
            std::ostream_iterator<std::string>(std::cout, "\n"));
    }
    catch(const std::exception &e)
    {
        std::cout << "Caught exception:\n";
        std::cout << "Type: " << typeid(e).name() << "\n";
        std::cout << "What: " << e.what() << "\n";
    }

    return 0;
}