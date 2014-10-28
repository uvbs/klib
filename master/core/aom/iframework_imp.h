#ifndef _klib_iframework_imp_h
#define _klib_iframework_imp_h


#pragma warning(disable: 4996)

#include "iframework.h"
#include "../core/singleton.h"

#include <vector>
#include <string.h>
#include <tchar.h>
#include <windows.h>

namespace klib{
namespace aom{



// 最大模块名称的长度
#define  MAX_MODULE_NAME_LENGTH 200


// 模块信息描述类
struct module_info
{
    imodule*    module_;
    TCHAR       name_[MAX_MODULE_NAME_LENGTH];
    module_info()  {  memset(this, 0, sizeof(*this));  }
};


// AOM框架实现
class framework : public iframework
{
protected:
    framework(framework&);
    framework& operator = (const framework&);
    framework()
    {
        m_vecInterfaceInfo.reserve(20);
    }


public:
    ~framework(void) {}

    static iframework* instance()
    {
        static framework _instance;
        return & _instance;
    }

    //----------------------------------------------------------------------
    // Summary:
    //		加载模块
    // Parameters:
    //		pszModuleFile		-	模块的路径(可以是相对路径)
    // Returns:
    //		成功返回S_OK，失败返回S_FALSE。
    //----------------------------------------------------------------------
    HRESULT load_module(LPCTSTR pszModuleFile) 
    {
        RegisterObjects_Fn RegisterObjects;
        HMODULE hModule = LoadLibrary(pszModuleFile);
        HRESULT hr = S_FALSE;

        if (hModule)
        {
            RegisterObjects = (RegisterObjects_Fn) GetProcAddress(hModule, "RegisterObjects");
            KLIB_ASSERT(RegisterObjects);
            if (nullptr == RegisterObjects)
            {
                return S_FALSE;
            }
            hr = RegisterObjects(AOM_LIB_VERSION, this);
        }

        return hr;
    }

    // 通过函数来加载模块
    HRESULT load_module(RegisterObjects_Fn RegisterObjects)
    {
        HRESULT hr = S_FALSE;

        if (nullptr == RegisterObjects)
        {
            return hr;
        }

        hr = RegisterObjects(AOM_LIB_VERSION, this);
        return hr;
    }

    //----------------------------------------------------------------------
    // Summary:
    //		卸载模块（接口设计中）
    // Parameters:
    //		pszModuleName		-	
    // Returns:
    //		
    //----------------------------------------------------------------------
    HRESULT unload_module(LPCTSTR pszModuleName)
    {
        KLIB_ASSERT(FALSE);

        return S_OK;
    }

    //----------------------------------------------------------------------
    // Summary:
    //		注册模块(将模块注册)
    // Parameters:
    //		pszModuleName		-	
    //		pModule		-	
    // Returns:
    //		
    //----------------------------------------------------------------------
    HRESULT register_module(LPCTSTR pszModuleName, imodule* pModule)
    {
        //接口可以重复添加
        KLIB_ASSERT(pModule);
        if (NULL == pModule) {
            return S_FALSE;
        }

        module_info info;
        _tcsncpy(info.name_, pszModuleName, _countof(info.name_));
        info.module_ = (imodule*) pModule;
        m_vecInterfaceInfo.push_back(info);

        ((module_imp*)pModule)->framwork_ = this;
        
        return S_OK;
    }

    //----------------------------------------------------------------------
    // Summary:
    //		在模块中查找接口
    // Parameters:
    //		guid		    -	
    //		pInteface		-	
    //      pModule         -   指示从pModule模块开始查找起
    // Returns:
    //		
    //----------------------------------------------------------------------
    imodule* find_next_interface(GUID guid, void** pInteface, imodule* pModule/* = NULL*/) 
    {
        InterfaceInfoVector::const_iterator itr;
        itr = m_vecInterfaceInfo.begin();
        HRESULT hr = S_FALSE;
        imodule* pFindedImodule = NULL;
        BOOL   bHasCheckedModule = (NULL == pModule ? TRUE : FALSE);

        for (; itr != m_vecInterfaceInfo.end(); ++ itr)
        {
            if (!bHasCheckedModule) {
                if (itr->module_ != pModule) {
                    continue;
                }
                bHasCheckedModule = TRUE;
                continue;
            }

            pFindedImodule = itr->module_;
            hr = pFindedImodule->find_interface(guid, (void**) pInteface);
            if (hr == S_OK)
            {
                return pFindedImodule;
            }
        }

        return NULL;
    }

    virtual HRESULT start() 
    {
        InterfaceInfoVector::const_iterator itr;
        itr = m_vecInterfaceInfo.begin();
        for (; itr != m_vecInterfaceInfo.end(); ++ itr)
        {
            itr->module_->on_pre_run();
        }

        itr = m_vecInterfaceInfo.begin();
        for (; itr != m_vecInterfaceInfo.end(); ++ itr)
        {
            if (!itr->module_->on_run())
            {
                this->stop();
                return S_FALSE;
            }
        }

        itr = m_vecInterfaceInfo.begin();
        for (; itr != m_vecInterfaceInfo.end(); ++ itr)
        {
            itr->module_->on_post_run();
        }

        return S_OK;
    }

    virtual HRESULT stop() 
    {
        InterfaceInfoVector::const_iterator itr;
        itr = m_vecInterfaceInfo.begin();
        for (; itr != m_vecInterfaceInfo.end(); ++ itr)
        {
            itr->module_->on_pre_stop();
        }

        itr = m_vecInterfaceInfo.begin();
        for (; itr != m_vecInterfaceInfo.end(); ++ itr)
        {
            itr->module_->on_stop();
        }

        itr = m_vecInterfaceInfo.begin();
        for (; itr != m_vecInterfaceInfo.end(); ++ itr)
        {
            itr->module_->on_post_stop();
        }

        return S_OK;
    }

protected:
    typedef std::vector<module_info>  InterfaceInfoVector;
    InterfaceInfoVector             m_vecInterfaceInfo;
};




}}


#endif
