////////////////////////////////////////////////////////////////////////////////
// @(#) util.h
// Utilities for std::string
// defined in namespace util
// by James Fancy
////////////////////////////////////////////////////////////////////////////////

#ifndef _klib_strutil_h
#define _klib_strutil_h

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stdlib.h>

namespace klib
{

// declaration
namespace util {
    
    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);

    std::string repeat(char c, int n);
    std::string repeat(const std::string& str, int n);

    bool startsWith(const std::string& str, const std::string& substr);
    bool endsWith(const std::string& str, const std::string& substr);
    bool equalsIgnoreCase(const std::string& str1, const std::string& str2);

    template<class T> T parseString(const std::string& str);
    template<class T> T parseHexString(const std::string& str);
    template<bool> bool parseString(const std::string& str);

    template<class T> std::string toString(const T& value);
    template<class T> std::string toHexString(const T& value, int width = 0);
    std::string toString(const bool& value);

    std::vector<std::string> split(const std::string& str, const std::string& delimiters);
}

// Tokenizer class
namespace util {
    class Tokenizer {
    public:
        static const std::string DEFAULT_DELIMITERS;
        Tokenizer(const std::string& str);
        Tokenizer(const std::string& str, const std::string& delimiters);

        bool nextToken();
        bool nextToken(const std::string& delimiters);
        const std::string getToken() const;

        /**
        * to reset the tokenizer. After reset it, the tokenizer can get
        * the tokens from the first token.
        */
        void reset();

    protected:
        size_t m_Offset;
        const std::string m_String;
        std::string m_Token;
        std::string m_Delimiters;
    };

}

// implementation of template functions
namespace util {

    template<class T> T parseString(const std::string& str) 
    {
        T value;
        std::istringstream iss(str);
        iss >> value;
        return value;
    }

    template<class T> T parseHexString(const std::string& str) 
    {
        T value;
        std::istringstream iss(str);
        iss >> hex >> value;
        return value;
    }

    template<class T> std::string toString(const T& value) 
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    template<class T> std::wstring toWString(const T& value) 
    {
        std::wostringstream oss;
        oss << value;
        return oss.str();
    }

    template<class T> std::string toHexString(const T& value, int width) 
    {
        std::ostringstream oss;
        oss << hex;
        if (width > 0) 
        {
            oss << setw(width) << setfill('0');
        }
        oss << value;
        return oss.str();
    }
}

namespace util
{
    template<typename TString>
    TString trimLeft(const TString& str) {
        TString t = str;
        TString::iterator i;
        for (i = t.begin(); i != t.end(); i++) {
            if (!isspace(*i)) {
                break;
            }
        }
        if (i == t.end()) {
            t.clear();
        } else {
            t.erase(t.begin(), i);
        }
        return t;
    }


    template<typename TString>
    TString trimRight(const TString& str) {
        if (str.begin() == str.end()) {
            return str;
        }

        TString t = str;
        TString::iterator i;
        for (i = t.end() - 1; ;i--) {
            if (!isspace(*i)) {
                t.erase(i + 1, t.end());
                break;
            }
            if (i == t.begin()) {
                t.clear();
                break;
            }
        }
        return t;
    }

    template<typename TString>
    TString trim(const TString& str) {
        TString t = str;

        TString::iterator i;
        for (i = t.begin(); i != t.end(); i++) {
            if (!isspace(*i)) {
                break;
            }
        }
        if (i == t.end()) {
            t.clear();
            return t;
        } else {
            t.erase(t.begin(), i);
        }

        for (i = t.end() - 1; ;i--) {
            if (!isspace(*i)) {
                t.erase(i + 1, t.end());
                break;
            }
            if (i == t.begin()) {
                t.clear();
                break;
            }
        }

        return t;
    }

    template<typename TString, typename TSep>
    std::vector<TString> Split(const TString& szText, TSep Separator = " ")
    {
        std::vector<TString> vec;
        TString str(szText);
        TString sep(Separator);
        size_t n = 0, nOld=0;
        while (n != TString::npos)
        {
            n = str.find(sep,n);
            if (n != TString::npos)
            {
                if (n != nOld)
                    vec.push_back(str.substr(nOld, n-nOld));
                n += sep.length();
                nOld = n;
            }
        }
        vec.push_back(str.substr(nOld, str.length()-nOld));

        return vec;
    }

    template<typename TString>
    std::vector<TString> Split(const TString& szText, char Separator = ' ')
    {
        TString sep(" ");
        sep[0] = Separator;
        return Split<TString, TString>(szText, sep);
    }

    template <typename TCont, typename TGlue>
    std::string Join(const TCont& container, const TGlue& glue)
    {
        std::string szResp("");
        typename TCont::const_iterator it;

        size_t s = container.size();
        for (it = container.begin(); it != container.end(); it++)
        {
            szResp += std::string(*it);
            if (--s)
                szResp += glue;
        }

        return szResp;
    }

    template <typename TCont>
    std::string Join(const TCont& container)
    {
        return Join(container, ' ');
    }

    template <typename TString, typename TSearch, typename TReplace>
    TString Replace(const TString& baseStr, const TSearch& searchStr, const TReplace& replaceStr)
    {
        TString szResp(baseStr);
        TString szSearch(searchStr);
        TString szReplace(replaceStr);
        if (searchStr == replaceStr)
            return szResp;

        size_t searchSize = szSearch.size();
        size_t replaceSize = szReplace.size();
        size_t n = 0;

        while (n != TString::npos)
        {
            n = szResp.find(szSearch, n);
            if (n != TString::npos)
            {
                szResp.replace(n, searchSize, szReplace);
                n += replaceSize;
            }
        }
        return szResp;
    }

    template <typename TString, typename TSearch>
    TString Replace(const TString& baseStr, const TSearch& searchStr, char replaceChar)
    {
        TString stdReplaceStr(" ");
        stdReplaceStr[0] = replaceChar;
        return Replace(baseStr, searchStr, stdReplaceStr);
    }

    template <typename TString, typename TReplace>
    TString Replace(const TString& baseStr, char searchChar, const TReplace& replaceStr)
    {
        TString stdSearchStr(" ");
        stdSearchStr[0] = searchChar;
        return Replace(baseStr, stdSearchStr, replaceStr);
    }

    template <typename TString>
    TString Replace(const TString& baseStr, char searchChar, char replaceChar)
    {
        TString stdSearchStr(" ");
        TString stdReplaceStr(" ");
        stdSearchStr[0] = searchChar;
        stdReplaceStr[0] = replaceChar;
        return Replace(baseStr, stdSearchStr, stdReplaceStr);
    }

    template <typename TString>
    TString ProperCase(const TString& str)
    {
        TString szResp(str);
        szResp[0] = std::toupper(szResp[0]);
        for (size_t i = 1; i < szResp.size(); i++)
            if (std::isspace(szResp[i-1]))
                szResp[i] = std::toupper(szResp[i]);
            else
                szResp[i] = std::tolower(szResp[i]);

        return szResp;
    }

    template <typename TString>
    TString ToUpper(const TString& str)
    {
        TString szResp(str);
        std::transform(szResp.begin(),szResp.end(), szResp.begin(), ::toupper);

        return szResp;
    }

    template <typename TString>
    TString ToLower(const TString& str)
    {
        TString szResp(str);
        std::transform(szResp.begin(),szResp.end(), szResp.begin(), ::tolower);

        return szResp;
    }


    //------------------------------------------------------------------------
    // other function 
    template<typename TString>
    TString extractFileExt(const TString& str)
    {
        static_assert(std::is_class<tstring>::value, "must be a class");
        typedef typename TString::traits_type::char_type char_type;

        auto pos = str.rfind(char_type('.'));
        if (pos == TString::npos) 
            return TString();
        else
            return str.substr(pos + 1);
    }

    template<typename TString>
    TString getMid(const TString& str,const typename TString::value_type* begin_str, const typename TString::value_type* end_str)
    {
        size_t begin_pos = str.find(begin_str);
        if (begin_pos == TString::npos) {
            return TString();
        }

        size_t end_pos = str.find(end_str, begin_pos + 1);
        if (end_pos == TString::npos) {
            return TString();
        }

        return str.substr(begin_pos, end_pos - begin_pos);
    }

    template<typename TString>
    TString replaceMid(const TString& str, 
        const typename TString::value_type* begin_str, 
        const typename TString::value_type* end_str,
        const typename TString::value_type* replace_str)
    {
        size_t begin_pos = str.find(begin_str);
        if (begin_pos == TString::npos) {
            return str;
        }

        size_t end_pos = str.find(end_str, begin_pos + 1);
        if (end_pos == TString::npos) {
            return str;
        }

        TString tmp = str;
        tmp.replace(begin_pos, end_pos - begin_pos + strlen(end_str), replace_str);

        return std::move(tmp);
    }

}}

#endif
