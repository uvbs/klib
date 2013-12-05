////////////////////////////////////////////////////////////////////////////////
// @(#) util.cpp
// Utilities for std::string
// defined in namespace util
// by James Fancy
////////////////////////////////////////////////////////////////////////////////
//#include "stdafx.h"
#include "strutil.h"
#include <algorithm>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>

namespace klib {
namespace util {

using namespace std;

        

std::string toLower(const std::string& str) {
    std::string t = str;
    transform(t.begin(), t.end(), t.begin(), tolower);
    return t;
}

std::string toUpper(const std::string& str) {
    std::string t = str;
    transform(t.begin(), t.end(), t.begin(), toupper);
    return t;
}

std::string repeat(char c, int n) {
    std::ostringstream s;
    s << std::setw(n) << std::setfill(c) << "";
    return s.str();
}

std::string repeat(const std::string& str, int n) {
    std::string s;
    for (int i = 0; i < n; i++) {
        s += str;
    }
    return s;
}

bool startsWith(const string& str, const string& substr) {
    return str.find(substr) == 0;
}

bool endsWith(const string& str, const string& substr) {
    size_t i = str.rfind(substr);
    return (i != string::npos) && (i == (str.length() - substr.length()));
}

bool equalsIgnoreCase(const string& str1, const string& str2) {
    return toLower(str1) == toLower(str2);
}

template<bool>
bool parseString(const std::string& str) {
    bool value;
    std::istringstream iss(str);
    iss >> boolalpha >> value;
    return value;
}

std::string toString(const bool& value) {
    ostringstream oss;
    oss << boolalpha << value;
    return oss.str();
}

std::vector<std::string> split(const std::string& str, const std::string& delimiters) {
    std::vector<std::string> ss;

    Tokenizer tokenizer(str, delimiters);
    while (tokenizer.nextToken()) {
        ss.push_back(tokenizer.getToken());
    }

    return ss;
}

}

namespace util {

const string Tokenizer::DEFAULT_DELIMITERS(" \t\n\r");

Tokenizer::Tokenizer(const std::string& str)
    : m_String(str), m_Offset(0), m_Delimiters(DEFAULT_DELIMITERS) {}

Tokenizer::Tokenizer(const std::string& str, const std::string& delimiters)
    : m_String(str), m_Offset(0), m_Delimiters(delimiters) {}

bool Tokenizer::nextToken() {
    return nextToken(m_Delimiters);
}

bool Tokenizer::nextToken(const std::string& delimiters) {
    // find the start charater of the next token.
    size_t i = m_String.find_first_not_of(delimiters, m_Offset);
    if (i == string::npos) {
        m_Offset = m_String.length();
        return false;
    }

    // find the end of the token.
    size_t j = m_String.find_first_of(delimiters, i);
    if (j == string::npos) {
        m_Token = m_String.substr(i);
        m_Offset = m_String.length();
        return true;
    }

    // to intercept the token and save current position
    m_Token = m_String.substr(i, j - i);
    m_Offset = j;
    return true;
}

const string Tokenizer::getToken() const {
    return m_Token;
}

void Tokenizer::reset() {
    m_Offset = 0;
}




}} // namespace