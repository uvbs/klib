#ifndef _dll_loader_H_
#define _dll_loader_H_


#ifdef  _MSC_VER
#include <Windows.h>
#else
#include <dlfcn.h>
#endif



#ifdef _MSC_VER
typedef   HMODULE  DLL_HANDLE;

#define LoadDll(dll_name)                   LoadLibraryA(dll_name)


#else


typedef   void *     DLL_HANDLE;
#define LoadDll(dll_name)                   dlopen(dll_name, RTLD_NOW)
#define GetProcAddress(handle, func_name)   dlsym(handle, func_name)
#define FreeLibrary(handle)                 dlclose(handle)
#endif


class dll_loader
{
public:
    dll_loader() :m_handle(0) { }
    ~dll_loader()
    {
        if (0 != m_handle) 
        {
            FreeLibrary(m_handle);
        }
    }

    DLL_HANDLE detach()
    {
        DLL_HANDLE tmp_handle = m_handle;
        m_handle = 0;

        return tmp_handle;
    }

    DLL_HANDLE attach(DLL_HANDLE handle)
    {
        DLL_HANDLE tmp_handle = m_handle;
        m_handle = handle;

        return tmp_handle;
    }

    bool load_lib(const char *dll_path)
    {
        m_handle = LoadDll(dll_path);
        if (NULL != m_handle) 
        {
            return true;
        }
#ifndef _MSC_VER
        else
        {
            printf("load %s failed: %s\r\n", dll_path, dlerror());
        }
#endif
        return false;
    }

    void* get_addr(const char* fun_name)
    {
        return GetProcAddress(m_handle, fun_name);
    }

protected:
    DLL_HANDLE m_handle;
};





#endif  // _COMMON_OSAA_DLL_