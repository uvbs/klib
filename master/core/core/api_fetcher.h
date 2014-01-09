#pragma once


#include <Windows.h>


template<typename apitype>
class api_fetcher
{
public:
    api_fetcher(LPCTSTR dllname, LPCSTR func_name) :
        dll_name_(dllname),
        func_name_(func_name)
    {
        hmodule_ = LoadLibrary(dll_name_);
    }
    ~api_fetcher()
    {
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

protected:
    LPCTSTR dll_name_;
    LPCSTR  func_name_;
    HMODULE hmodule_;
};