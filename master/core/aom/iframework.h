#ifndef _klib_iframework_h
#define _klib_iframework_h


namespace klib {
namespace aom {



/* AOM框架的版本号 */
#define   AOM_LIB_VERSION  (1<<4)

#define   KLIB_ASSERT(expression)                             \
        _ASSERT(expression)

/* 注册模块宏定义 */
#define  BEGIN_REGISTER_MODULES     \
    EXTERN_C                                                                    \
    {                                                                           \
        _declspec(dllexport)                                                    \
        HRESULT RegisterObjects(UINT uLibVersion, klib::aom::iframework* pIServer)      \
        {                                                                       \
            if (uLibVersion != AOM_LIB_VERSION)                                 \
            {                                                                   \
                return S_FALSE;                                                 \
            }

#define REGISTER_MODULE(ModuleName)   \
            pIServer->register_module(TEXT(#ModuleName), new ModuleName);

#define END_REGISTER_MODULES                                                    \
            return S_OK;                                                        \
        }                                                                       \
    };


/* 模块内部的接口申明 */
#define  BEGIN_INTERFACE_ENTRY                                          \
    HRESULT find_interface(GUID guid, void** pPointer)                  \
    {

#define INTERFACE_ENTRY(_theGUID, _theINTERFACE)                        \
    if (guid == _theGUID)                                               \
    {                                                                   \
        if (pPointer)                                                   \
            *pPointer = (_theINTERFACE*) this;                          \
        return S_OK;                                                    \
    }

#define END_INTERFACE_ENTRY                                             \
        return S_FALSE;                                                 \
    }

/* 模块接口定义 */
struct iframework;
struct imodule
{
    virtual iframework* frame() = 0;

    /**
    * @brief 引擎运行事件之准备运行
    * 
    *     分析模块可以在这个事件中查询依赖的模块和接口，初始化系统变量
    */
    virtual void on_pre_run() = 0;

    /**
    * @brief   引擎运行事件之运行
    *
    *   	如果这个运行的必须资源不成功，则返回FALSE，否则返回TRUE
    *
    *		如果运行失败，一定要调用GetEngineServer()->SetLastErrorText()方法设置错误原因
    *       分析模块可以在这个事件中申请系统资源，创建线程，检查依赖的接口等。
    */
    virtual BOOL on_run() = 0;

    /**
    * @brief  引擎运行事件之运行之后
    *
    *     分析模块可以在这个事件中进行数据的最后处理，如创建定时器...
    */
    virtual void on_post_run() = 0;

    /**
    * @brief   引擎运行事件之准备停止
    *
    *     分析模块可以在这个事件中做准备停止的操作，如果设置线程事件信号，停止定时器
    */
    virtual void on_pre_stop() = 0;

    /**
    * @brief  引擎运行事件之停止
    *
    *     分析模块可以在这个事件中结束线程
    */
    virtual void on_stop() = 0;

    /**
    * @brief    引擎运行事件之停止之后
    *
    *     分析模块可以在这个事件中释放系统资源
    */
    virtual void on_post_stop() = 0;

    /**
    * @brief   根据接口GUID查找相应的接口
    *
    *		guid		-	
    *		pInterface		-	
    * Returns:
    *		
    */
    virtual HRESULT find_interface(GUID guid, void** pInterface) = 0;
};

class module_imp : public imodule
{
public:
    module_imp() : framwork_(NULL) {}
    iframework* frame()
    {
        return framwork_;
    }

    virtual void on_pre_run()
    {
    }
    virtual BOOL on_run()
    {
        return TRUE;
    }
    virtual void on_post_run()
    {
    }
    virtual void on_pre_stop()
    {
    }
    virtual void on_stop()
    {
    }
    virtual void on_post_stop()
    {
    }

public:
    iframework* framwork_;
};

typedef HRESULT (*RegisterObjects_Fn)(UINT uLibVersion, iframework* pIAomServer) ;

/**  AOM框架服务接口  */
struct iframework
{
public:
    // 加载模块
    virtual HRESULT load_module(LPCTSTR pszModuleFile) = 0;

    // 卸载模块
    virtual HRESULT unload_module(LPCTSTR pszModuleName) = 0;

    // 注册模块
    virtual HRESULT register_module(LPCTSTR pszModuleName, imodule* pModule) = 0;

    // 查找接口功能
    virtual imodule* find_next_interface(GUID guid, void** pInteface, imodule* pModule = NULL) = 0;

    // 开启服务
    virtual HRESULT start() = 0;

    // 停止服务
    virtual HRESULT stop() = 0;
};



}}


#endif
