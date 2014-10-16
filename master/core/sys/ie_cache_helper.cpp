#include "ie_cache_helper.h"


#include <WinInet.h>
#include <Urlhist.h>
#include <ShlGuid.h>

#pragma comment(lib, "Wininet.lib")


namespace klib{
namespace sys{



ie_cache_helper::ie_cache_helper(void)
{
}


ie_cache_helper::~ie_cache_helper(void)
{
}

BOOL ie_cache_helper::delete_url_cache(e_del_cache_type type)
{
    BOOL bRet = FALSE;
    HANDLE hEntry;
    LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry = NULL;  
    DWORD dwEntrySize;

    //delete the files
    dwEntrySize = 0;
    hEntry = FindFirstUrlCacheEntry(NULL, NULL, &dwEntrySize);
    lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) new char[dwEntrySize];
    hEntry = FindFirstUrlCacheEntry(NULL, lpCacheEntry, &dwEntrySize);
    if (!hEntry)
    {
        goto cleanup;
    }

    do
    {
        if (type == e_cache_file &&
            !(lpCacheEntry->CacheEntryType & COOKIE_CACHE_ENTRY))
        {
            DeleteUrlCacheEntry(lpCacheEntry->lpszSourceUrlName);
        }
        else if (type == e_cache_cookie &&
            (lpCacheEntry->CacheEntryType & COOKIE_CACHE_ENTRY))
        {
            DeleteUrlCacheEntry(lpCacheEntry->lpszSourceUrlName);
        }

        dwEntrySize = 0;
        FindNextUrlCacheEntry(hEntry, NULL, &dwEntrySize);
        delete [] lpCacheEntry; 
        lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) new char[dwEntrySize];
    }
    while (FindNextUrlCacheEntry(hEntry, lpCacheEntry, &dwEntrySize));

    bRet = TRUE;
cleanup:
    if (lpCacheEntry)
    {
        delete [] lpCacheEntry; 
    }
    return bRet;
}

BOOL ie_cache_helper::delete_ie_cache(BOOL bDeleteCache /*= TRUE*/, BOOL bDeleteCacheIndex /*= FALSE*/)
{
    TCHAR szUserProfile[200]; 
    TCHAR szFilePath[200];
    HANDLE hCacheEnumHandle  = NULL;
    LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry = NULL;
    DWORD dwSize = 4096; // initial buffer size

    // Delete index.dat if requested.
    // Be sure that index.dat is not locked.
    if (bDeleteCacheIndex)
    {
        // Retrieve from environment user profile path.
        ExpandEnvironmentStrings("%userprofile%", szUserProfile, 
            sizeof(szUserProfile)); 
        wsprintf(szFilePath, "%s%s", szUserProfile, 
            "\\Local Settings\\Temporary Internet" 
            " Files\\Content.IE5\\index.dat");

        DeleteFile(szFilePath);

        if (!bDeleteCache) return TRUE;
    }

    // Enable initial buffer size for cache entry structure. 
    lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) new char[dwSize];
    lpCacheEntry->dwStructSize = dwSize;

    // URL search pattern (1st parameter)
    // options are:  NULL ("*.*"), "cookie:"  
    // or "visited:". 
    hCacheEnumHandle = FindFirstUrlCacheEntry(NULL /* in */ , 
        lpCacheEntry /* out */, 
        &dwSize /* in, out */);

    // First, obtain handle to internet cache
    // with FindFirstUrlCacheEntry 
    // for later use with FindNextUrlCacheEntry. 
    if (hCacheEnumHandle != NULL) 
    {
        // When cache entry is not a cookie, delete entry. 
        if (!(lpCacheEntry->CacheEntryType & COOKIE_CACHE_ENTRY))
        {            
            DeleteUrlCacheEntry(lpCacheEntry->lpszSourceUrlName);
        }
    }
    else
    {
        switch (GetLastError())
        {
        case ERROR_INSUFFICIENT_BUFFER:
            lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) new char[dwSize];
            lpCacheEntry->dwStructSize = dwSize;

            // Repeat first step search with adjusted buffer, exit if not
            // found again (in practice one buffer's size adustment is  
            // always OK).
            hCacheEnumHandle = FindFirstUrlCacheEntry(NULL, lpCacheEntry, 
                &dwSize);
            if (hCacheEnumHandle != NULL) 
            {
                // When cache entry is not a cookie, delete entry. 
                if (!(lpCacheEntry->CacheEntryType & COOKIE_CACHE_ENTRY))
                {            
                    DeleteUrlCacheEntry(lpCacheEntry->lpszSourceUrlName);
                }
                break;        
            }
            else
            {
                // FindFirstUrlCacheEntry fails again, return.
                return FALSE; 
            }
        default:
            FindCloseUrlCache(hCacheEnumHandle);
            return FALSE;
        }
    }

    // Next, use hCacheEnumHandle obtained
    // from the previous step to delete 
    // subsequent items of cache.
    do 
    {
        // Notice that return values of FindNextUrlCacheEntry (BOOL) and 
        // FindFirstUrlCacheEntry (HANDLE) are different.
        if (FindNextUrlCacheEntry(hCacheEnumHandle, lpCacheEntry, &dwSize))
        {
            // When cache entry is not a cookie, delete entry. 
            if (!(lpCacheEntry->CacheEntryType & COOKIE_CACHE_ENTRY))
            {            
                DeleteUrlCacheEntry(lpCacheEntry->lpszSourceUrlName);
            }
        }
        else
        {
            switch(GetLastError())
            {
            case ERROR_INSUFFICIENT_BUFFER:
                lpCacheEntry = 
                    (LPINTERNET_CACHE_ENTRY_INFO) new char[dwSize];
                lpCacheEntry->dwStructSize = dwSize;

                // Repeat next step search with adjusted buffer, exit if 
                // error comes up again ((in practice one buffer's size 
                // adustment is always OK).
                if (FindNextUrlCacheEntry(hCacheEnumHandle, lpCacheEntry, 
                    &dwSize)) 
                {
                    // When cache entry is not a cookie, delete entry. 
                    if (!(lpCacheEntry->CacheEntryType & 
                        COOKIE_CACHE_ENTRY))
                    {
                        DeleteUrlCacheEntry(
                            lpCacheEntry->lpszSourceUrlName);
                    }
                    break;
                }
                else
                {
                    // FindFirstUrlCacheEntry fails again, return.
                    FindCloseUrlCache(hCacheEnumHandle);
                    return FALSE; 
                }
                break;
            case ERROR_NO_MORE_ITEMS:
                FindCloseUrlCache(hCacheEnumHandle);
                return TRUE; 
            default:
                FindCloseUrlCache(hCacheEnumHandle);
                return FALSE;
            }
        }
    } while (TRUE);
}

HRESULT ie_cache_helper::delete_ie_history(BOOL bDeleteHistory /*= TRUE*/, 
    BOOL bDeleteHistoryIndex /*= FALSE*/) 
{
    TCHAR szUserProfile[200];
    TCHAR szFilePath[200];     
    HRESULT hr;

    // Delete index.dat if requested.
    // Be sure that index.dat is not locked. 
    if (bDeleteHistoryIndex)
    {
        // Retrieve from environment user profile path. 
        ExpandEnvironmentStrings("%userprofile%", szUserProfile, 
            sizeof(szUserProfile)); 
        wsprintf(szFilePath, "%s%s", szUserProfile, 
            "\\Local Settings\\History" 
            "\\History.IE5\\index.dat");
        DeleteFile(szFilePath);

        if (!bDeleteHistoryIndex) return S_OK;
    } 

    CoInitialize(NULL);

    IUrlHistoryStg2* pUrlHistoryStg2 = NULL;
    hr = CoCreateInstance(CLSID_CUrlHistory, NULL, CLSCTX_INPROC, 
        IID_IUrlHistoryStg2, 
        (void**)&pUrlHistoryStg2);
    if (SUCCEEDED(hr))
    {
        hr = pUrlHistoryStg2->ClearHistory();
        pUrlHistoryStg2->Release();
    }


    return hr;
}



}}