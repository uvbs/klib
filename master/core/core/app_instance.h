#pragma once

#include <windows.h>


/**
 * @brief 判断是否有前一个实例存在
 */

namespace klib {
namespace core {


class app_instance
{
public:
    app_instance(LPCTSTR pszMetuxName) : m_bexist_prev_instances(false)
    {
        HANDLE hMetux = CreateMutex(NULL, FALSE, pszMetuxName);
        if (GetLastError() == ERROR_ALREADY_EXISTS) 
        {
            CloseHandle(hMetux);
            m_bexist_prev_instances = true;
        }
    }

    /**
     * @brief 返回是否有前一个实例存在
     */
    bool is_exist_prev_instance()
    {
        return m_bexist_prev_instances;
    }

    /**
     * @brief 同上，返回判断是否具有前一个实例
     */
    operator bool ()
    {
        return is_exist_prev_instance();
    }

protected:
    bool  m_bexist_prev_instances;
};


}}
