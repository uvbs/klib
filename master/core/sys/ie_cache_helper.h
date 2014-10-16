#pragma once

#include <windows.h>

enum e_del_cache_type //要删除的类型。
{
    e_cache_file,       //表示internet临时文件
    e_cache_cookie,     //表示Cookie
};

class ie_cache_helper
{
public:
    ie_cache_helper(void);
    ~ie_cache_helper(void);


    //以下为一些，辅助函数.
    BOOL delete_url_cache(e_del_cache_type type);

    BOOL delete_ie_cache(BOOL bDeleteCache = TRUE, BOOL bDeleteCacheIndex = FALSE);

    HRESULT delete_ie_history(BOOL bDeleteHistory = TRUE, 
        BOOL bDeleteHistoryIndex = FALSE) ;
};

