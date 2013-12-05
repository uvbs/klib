#pragma once
#include <string>
#include <codecvt>

namespace klib {
namespace util {


using std::to_string;


template<class T>
std::string to_string(const T& t)
{
    ostringstream oss;
    oss << t;
    return std::string(oss.str());
}

// template<>
// std::string to_string<const std::wstring>(const std::wstring& val)
// {
//     std::mbstate_t state = std::mbstate_t();
//     const wchar_t* wstr =  val.c_str();
//     size_t len = 1 + std::wcsrtombs(NULL, &wstr, 0, &state);
//     std::vector<char> mbstr(len);
//     std::wcsrtombs(&mbstr[0], &wstr, mbstr.size(), &state);
// 
//     return std::string(&mbstr[0]);
// }
    


// 
// template<UINT32>
// std::string to_string(UINT32 &t)
// {
//     return std::string("");
// }
























}}

