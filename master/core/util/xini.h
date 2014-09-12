#ifndef _csdcs_ini_h_
#define _csdcs_ini_h_

#include <string>
#include <stdio.h>
#include <strstream>

#include "../inttype.h"

namespace klib{
namespace util{


class xini
{
public:
    xini(void);
    ~xini(void);

public:
    bool set_file(const std::string& str);
    bool write_str(const std::string& section, const std::string& key, const std::string& val);
    template<typename T>
    bool write_val(const std::string& section, const std::string& key, const T& val);

    bool read_str(const std::string& section, const std::string& key, std::string& val);
    bool read_bool(const std::string& section, const std::string& key, bool& bval);
    bool read_uint64(const std::string& section, const std::string& key, UINT64& ival);
    template<typename T>
    bool read_val(const std::string& section, const std::string& key, T& val);

protected:
    static std::string trim(const std::string &strInput);

    std::string extract_section(const std::string& str);
    std::string extract_key(const std::string& str);
    std::string extract_value(const std::string& str);

    const char* skip_space(const char*& p);
    const char* skip_until_line(const char* p);

    void parser_pos(const char* secion, const char*key,
        const char*& section_begin,
        const char*& section_end,
        const char*& key_begin,
        const char*& key_end,
        const char*& val_begin,
        const char*& val_end);
    size_t parsed_section(const char* p);    
    size_t parsed_key(const char* p);
    size_t parsed_value(const char* p);
    size_t parsed_comment(const char* p);
    size_t parsed_line(const char* p);

protected:
    UINT64 parsed_data(const char* str);

protected:
    std::string m_str_content;
    std::string m_str_file;
};


template<typename T>
bool xini::write_val(const std::string& section, const std::string& key, const T& val)
{
    std::stringstream oss;
    oss << val;

    std::string str_val = oss.str();
    return write_str(section, key, str_val);
}

template<typename T>
bool xini::read_val(const std::string& section, const std::string& key, T& val)
{
    std::string str_val;
    bool ret = read_str(section, key, str_val);
    if (!ret)
    {
        return false;
    }

    std::stringstream oss;
    oss << str_val;
    oss >> val;

    return true;
}


}}


#endif // _csdcs_ini_h_

