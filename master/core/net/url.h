#ifndef __klib_url_h__
#define __klib_url_h__


#include <string>

namespace klib{
namespace net{

class url_parser
{
public:
    url_parser(const char* url);
    const std::string get_scheme() const
    {
        return m_scheme;
    }
    const std::string get_host() const
    {
        return m_host;
    }
    const std::string get_port() const
    {
        return m_port;
    }
    const std::string get_path() const
    {
        return m_path;
    }
    const std::string get_query() const
    {
        return m_query;
    }
    const std::string get_fragment() const
    {
        return m_fragment;
    }
    const std::string get_username() const
    {
        return m_username;
    }
    const std::string get_password() const
    {
        return m_password;
    }
private:
    bool init(const char* url);
    bool is_scheme_char(int c)
    {
        return (!isalpha(c) && '+' != c && '-' != c && '.' != c) ? 0 : 1;
    }
private:
    std::string m_scheme;               /* mandatory */
    std::string m_host;                 /* mandatory */
    std::string m_port;                 /* optional */
    std::string m_path;                 /* optional */
    std::string m_query;                /* optional */
    std::string m_fragment;             /* optional */
    std::string m_username;             /* optional */
    std::string m_password;             /* optional */
};

}}

#endif // __klib_url_h__