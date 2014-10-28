#ifndef _klib_api_fetcher_h
#define _klib_api_fetcher_h


#include <Windows.h>


namespace klib {
namespace core {


template<typename apitype>
class api_fetcher
{
public:
    api_fetcher(LPCTSTR dllname, LPCSTR func_name) 
        : dll_name_(dllname)
        , func_name_(func_name)
    {
        hmodule_ = LoadLibrary(dll_name_);
    }

    ~api_fetcher()
    {
        if (hmodule_)
            FreeLibrary(hmodule_);
    }

    operator apitype () {
        apitype f = (apitype) GetProcAddress(hmodule_, func_name_);
        return f;
    }

    apitype operator * ()
    {
        apitype f = (apitype) GetProcAddress(hmodule_, func_name_);
        return f;
    }

    HMODULE detach()
    {
        HMODULE mod = hmodule_;
        hmodule_ = nullptr;

        return mod;
    }

protected:
    LPCTSTR dll_name_;
    LPCSTR  func_name_;
    HMODULE hmodule_;
};



}}



#endif
