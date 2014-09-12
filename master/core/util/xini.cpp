
#include "xini.h"

using namespace klib::util;


xini::xini(void) 
{
}

xini::~xini(void)
{
}

bool xini::set_file(const std::string& str)
{
    m_str_content.clear();
    m_str_file = str;
    FILE* hFile = fopen(str.c_str(), "rb");
    if (NULL == hFile) 
    {
        return false;
    }

    char buff[1024];
    while (!feof(hFile)) 
    {
        size_t nreaded = fread(buff, 1, sizeof(buff), hFile);
        if (nreaded > 0) 
        {
            m_str_content.append(buff, nreaded);
        }
    }

    fclose(hFile);

    return true;
}

bool xini::write_str(const std::string& section, const std::string& key, const std::string& val)
{
    const char* section_begin = nullptr;
    const char* section_end = nullptr;
    const char* key_begin = nullptr;
    const char* key_end = nullptr;
    const char* val_begin = nullptr;
    const char* val_end = nullptr;

    parser_pos(section.c_str(), key.c_str(), 
        section_begin, section_end,
        key_begin, key_end,
        val_begin, val_end);

    if (nullptr == section_begin)  // 没有section
    {
        // 在最后面添加section 
        std::string str_to_add = "\r\n[";
        str_to_add.append(section);
        str_to_add.append("]\r\n");

        str_to_add.append(key);
        str_to_add.append("=");
        str_to_add.append(val);
        str_to_add.append("\r\n");

        m_str_content.append(str_to_add);
    }
    else if (nullptr == key_begin) // 没有key
    {
        // 在section 后面添加key
        std::string str_to_add = "\r\n";
        str_to_add.append(key);
        str_to_add.append("=");
        str_to_add.append(val);
        str_to_add.append("\r\n");

        size_t start_pos = section_end   - m_str_content.c_str();

        m_str_content.replace(start_pos, 0, str_to_add);
    }
    else if (nullptr == val_begin) // 没有 val
    {
        // 在key 后面添加key
        std::string str_to_add = "=";
        str_to_add.append(val);

        size_t start_pos = key_end   - m_str_content.c_str();

        m_str_content.replace(start_pos, 0, str_to_add);
    }
    else 
    {
        // 都有的情况下，替换val
        std::string str_to_add = val;

        size_t start_pos = val_begin   - m_str_content.c_str();
        size_t end_pos   = val_end     - m_str_content.c_str();

        m_str_content.replace(start_pos, end_pos - start_pos, str_to_add);
    }

    FILE* hFile = fopen(m_str_file.c_str(), "wb");
    if (NULL == hFile) 
    {
        return false;
    }
    fwrite(m_str_content.c_str(), 1, m_str_content.size(), hFile);
    fclose(hFile);

    return true;
}

bool xini::read_str(const std::string& section, const std::string& key, std::string& val)
{
    const char* section_begin = nullptr;
    const char* section_end = nullptr;
    const char* key_begin = nullptr;
    const char* key_end = nullptr;
    const char* val_begin = nullptr;
    const char* val_end = nullptr;

    parser_pos(section.c_str(), key.c_str(), 
        section_begin, section_end,
        key_begin, key_end,
        val_begin, val_end);

    if (val_begin && val_end)
    {
        val = std::string(val_begin, val_end);
        return true;
    }
    
    return false;
}

bool xini::read_bool(const std::string& section, const std::string& key, bool& bval)
{
    std::string str;
    bool ret = read_str(section, key, str);
    if (!ret) 
    {
        return false;
    }

    bval = false;
    if (_stricmp(str.c_str(), "true") == 0 ||
        strcmp(str.c_str(), "1") == 0) 
    {
        bval = true;
    }

    return true;
}

bool xini::read_uint64(const std::string& section, const std::string& key, UINT64& ival)
{
    std::string str;
    bool ret = read_str(section, key, str);
    if (!ret) 
    {
        return false;
    }

    ival = parsed_data(str.c_str());

    return true;
}

std::string xini::trim(const std::string &strInput)
{
    size_t nSize = strInput.length();
    size_t nl = 0;
    size_t nr = nSize-1;

    if (0 == nSize)
    {
        return strInput;
    }

    for (nl = 0; nl < nSize; ++nl )
    {
        if (!isspace(strInput[nl]))
        {
            break;
        }
    }

    if (nl < nSize)
    {
        for (nr = nSize-1; nr > nl; --nr)
        {
            if (!isspace(strInput[nr]))
            {
                break;
            }
        }
    }
    
    std::string strOutput;
    strOutput.assign(strInput.c_str()+nl, nr-nl+1);
    return strOutput;
}

std::string xini::extract_section(const std::string& str)
{
    std::string str_sec = str;
    str_sec.erase(str_sec.begin());
    str_sec.erase(-- str_sec.end());

    return trim(str_sec);
}

std::string xini::extract_key(const std::string& str)
{
    size_t pos = str.find('=');
    if (pos != std::string::npos) 
    {
        std::string key = str.substr(0, pos);
        return trim(key);
    }

    return trim(str);
}

std::string xini::extract_value(const std::string& str)
{
    const char* p = str.c_str();
    p = skip_until_line(p);

    return std::string(str.c_str(), p);
}

const char* xini::skip_space(const char*& p)
{
    while (*p && *p <= 0x20) 
    {
        // 回车换行不跳过
        if (*p == '\n' || *p == '\r') 
        {
            break;
        }
        ++ p;
    }

    return p;
}

const char* xini::skip_until_line(const char* p)
{
    size_t len;
    while (*p && (len = this->parsed_line(p)) == 0)
    {
        ++ p;
    }

    return p;
}

void xini::parser_pos(const char* section, const char*key,
        const char*& section_begin,
        const char*& section_end,
        const char*& key_begin,
        const char*& key_end,
        const char*& val_begin,
        const char*& val_end)
{
    section_begin = section_end = key_begin = key_end = val_begin = val_end;

    bool is_new_line = true;
    bool is_section_match = false;
    const char* cur_pos = m_str_content.c_str();
    const char* end_pos = m_str_content.c_str() + m_str_content.size();
    size_t nlen = 0;

    while (cur_pos < end_pos) 
    {
        nlen = this->parsed_comment(cur_pos);
        if (nlen > 0) 
        {
            cur_pos += nlen;
        }

        // section
        nlen = this->parsed_section(cur_pos);
        if (nlen > 0 && is_new_line) 
        {
            // 如果已经有匹配的section，又出现了section的话，则认为是上一个section的结束点
            if (is_section_match)
            {
                section_end = cur_pos;
            }

            // 提取section
            std::string str_sec = extract_section(std::string(cur_pos, nlen));
            if (str_sec.compare(section) == 0) 
            {
                section_begin = cur_pos;
                section_end   = end_pos; // 赋初值为结束位置

                is_section_match = true;
            }

            is_new_line = false;
        }
        cur_pos += nlen;

        if (nlen > 0) 
        {
            cur_pos = this->skip_until_line(cur_pos);// 直接跳过该行
        }

        skip_space(cur_pos);

        // key
        nlen = this->parsed_key(cur_pos);
        if (nlen > 0) 
        {
            if (is_section_match) 
            {
                std::string str_key = extract_key(std::string(cur_pos, nlen));
                if (str_key == key) 
                {
                    key_begin = cur_pos;
                    key_end   = cur_pos + str_key.size();

                    auto tmp_ = cur_pos + nlen;
                    skip_space(tmp_);
                    if (*tmp_ == '=') 
                    {
                        ++ tmp_;

                        val_begin = tmp_;

                        skip_space(tmp_);
                        tmp_ = skip_until_line(tmp_);

                        val_end = tmp_;
                    }
                    return;
                }
            }

            cur_pos += nlen;

            // 直接跳过该行
            cur_pos = this->skip_until_line(cur_pos);
        }

        skip_space(cur_pos);

        // line
        nlen = this->parsed_line(cur_pos);
        if (nlen > 0) 
        {
            is_new_line = true;
            cur_pos += nlen;
        }
    }
}

size_t xini::parsed_section(const char* p)
{
    const char* old_pos = p;

    if (*p != '[') 
    {
        return 0;
    }
    ++ p;

    skip_space(p);

    while (isalpha(*p) || isdigit(*p) || *p == '_') ++ p;

    skip_space(p);

    if (*p == ']') 
    {
        return (p - old_pos) + 1;
    }

    return 0;
}

size_t xini::parsed_key(const char* p)
{
    const char* old_ptr = p;
    while (isalpha(*p) || isdigit(*p) || *p == '_')
        ++ p;

    return p - old_ptr;
}

size_t xini::parsed_value(const char* p)
{
    const char* old_ptr = p;
    size_t nLen = 0;
    while ((nLen = this->parsed_line(p)) == 0) 
        ++ p;

    return p - old_ptr;
}

size_t xini::parsed_comment(const char* p)
{
    const char* old_ptr = p;
    if ('#' ==  *p || ';' == *p) 
    {
        p = skip_until_line(p);
        return p - old_ptr;
    }

    if (_strnicmp(p, "rem ", 3) == 0) 
    {
        p = skip_until_line(p);
        return p - old_ptr;
    }

    return 0;
}

size_t xini::parsed_line(const char* p)
{
    if (*p == '\n') 
    {
        return 1;
    }
    if (*p == '\r' && *(p + 1) == '\n') 
    {
        return 2;
    }
    return 0;
}

UINT64 xini::parsed_data(const char* str)
{
    const int base_16[] = {10,11,12,13,14,15};
    int base = 10;
    UINT64  ret = 0;
    
    str = skip_space(str);

    if (_strnicmp(str, "0x", 2) == 0)
    {
        base = 16;
    }

    for(int i=0; str[i]!='\0' && !isspace(str[i]); i++)
    {
        ret *= base;

        char ch = str[i];
        if(ch >= '0' && ch <= '9')
            ret += str[i]-'0';
        else if (ch >= 'A' && ch <= 'F')
            ret += base_16[ch - 'A'];
        else if (ch >= 'a' && ch <= 'f')
            ret += base_16[ch - 'a'];
    }

    return ret;
}