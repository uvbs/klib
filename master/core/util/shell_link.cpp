//#include "stdafx.h"
#include "shell_link.h"

//#include <shobjidl.h>
#include <COMDEF.H>
#include <ShlGuid.h>
#include <SHLOBJ.H>
#include <tchar.h>


namespace klib {
namespace util {



shell_link::shell_link(void)
{

}

shell_link::~shell_link(void)
{
}

BOOL shell_link::create_short_cut(const LPCTSTR FilePath,const LPCTSTR LinkPath, LPCTSTR pszDesc /*= NULL*/)  
{  
    BOOL ret = false;
    IShellLink *psl; 
    ::CoInitialize(NULL);
    if(SUCCEEDED(CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(LPVOID*)&psl)))  
    {  
        IPersistFile *ppf;  
        psl->SetPath(FilePath);  
        psl->SetDescription(pszDesc ? pszDesc :_T("Shortcut"));  
        psl->SetShowCmd(SW_SHOW);  
        if(SUCCEEDED(psl->QueryInterface(IID_IPersistFile,(LPVOID*)&ppf)))  
        {
#ifndef _UNICODE
            WORD wsz[MAX_PATH*2];
            MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, LinkPath, -1, wsz, _countof(wsz));

            if(SUCCEEDED(ppf->Save(wsz, true)))
                ret = true;
#else

            if(SUCCEEDED(ppf->Save(LinkPath, true)))
                ret = true;
#endif

            ppf->Release();  
        }  
        psl->Release();  
    }  
    return ret;  
}   


tstring shell_link::get_link_target(const tstring& src_file)
{
    HRESULT hres;
    IShellLink* pShellLink;
    WIN32_FIND_DATA wfd;

    TCHAR szGotPath[MAX_PATH];

    CoInitialize(NULL); 
    hres = CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(VOID**)&pShellLink);
    if (SUCCEEDED(hres))
    {
        IPersistFile* ppf;
        hres = pShellLink->QueryInterface(IID_IPersistFile,(VOID**)&ppf);
        if (SUCCEEDED(hres))
        {

            wchar_t szLinkPath[2048] = {0};
#ifdef _UNICODE
            wcscpy_s(szLinkPath, _countof(szLinkPath), src_file.c_str());
#else
            wcscpy_s(szLinkPath,MAX_STRING_BUFFER,CLibX::String::GBToUTF8(lpszFile).c_str());
#endif // _UNICODE

            hres = ppf->Load(szLinkPath,STGM_READ);
            if (SUCCEEDED(hres))
            {
                int nIconIndex = 0 ;
                hres = pShellLink->GetPath(szGotPath,MAX_PATH,(WIN32_FIND_DATA*)&wfd,SLGP_RAWPATH);

                // 读取快捷方式的打开方式
                if (SUCCEEDED(hres))
                {
                    ppf->Release();
                    return szGotPath;
                }
            }
        }
        ppf->Release();
    }
    CoUninitialize();
    return _T("");
}



}}