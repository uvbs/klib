
#ifndef _fnmatch_h_
#define _fnmatch_h_

enum MatchFlags
{
    // Wildcards in string cannot match `/', match filename only
    // *.c not match dir/name.c
    MATCH_FILE_NAME_ONLY = 1<<0,

    // Don't treat the `\' character as escape
    NO_ESCAPE = 1<<1,

    // Not match `.' as the first character of string
    MATCH_PERIOD = 1<<2,

    // Match leading dir part
    MATCH_LEADING_DIR = 1<<3,

    // Ignore case
    // *.c match filename.C
    IGNORE_CASE = 1<<4,
};

// 匹配结果
enum e_match_result
{
    FNM_NOMATCH    = 1 << 1,
    FNM_FILE_NAME  = 1 << 2,
    FNM_NOESCAPE   = 1 << 3,
    FNM_PERIOD     = 1 << 4,
    FNM_LEADING_DIR= 1 << 5,
    FNM_CASEFOLD   = 1 << 6,
};

// 查看是否匹配，返回0表示匹配
int fnmatch(const char pattern, const char string, int flags = 0);


#endif