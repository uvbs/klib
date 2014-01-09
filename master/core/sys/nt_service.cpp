#include "stdafx.h"
#include "nt_service.h"
#include "sys_dir_helper.h"
#include "api_fetcher.h"

//----------------------------------------------------------------------
typedef 
    SC_HANDLE
    (WINAPI
    *CreateServiceA_T)(
        SC_HANDLE    hSCManager,
        LPCSTR     lpServiceName,
        LPCSTR     lpDisplayName,
        DWORD        dwDesiredAccess,
        DWORD        dwServiceType,
        DWORD        dwStartType,
        DWORD        dwErrorControl,
        LPCSTR     lpBinaryPathName,
        LPCSTR     lpLoadOrderGroup,
        LPDWORD      lpdwTagId,
        LPCSTR     lpDependencies,
        LPCSTR     lpServiceStartName,
        LPCSTR     lpPassword
    );

typedef 
    SC_HANDLE
    (WINAPI
    *CreateServiceW_T)(
        SC_HANDLE    hSCManager,
        LPCWSTR     lpServiceName,
        LPCWSTR     lpDisplayName,
        DWORD        dwDesiredAccess,
        DWORD        dwServiceType,
        DWORD        dwStartType,
        DWORD        dwErrorControl,
        LPCWSTR     lpBinaryPathName,
        LPCWSTR     lpLoadOrderGroup,
        LPDWORD      lpdwTagId,
        LPCWSTR     lpDependencies,
        LPCWSTR     lpServiceStartName,
        LPCWSTR     lpPassword
    );

#ifdef _UNICODE
#define CreateService_T CreateServiceW_T
static api_fetcher<CreateService_T> CreateService_f(_T("Advapi32.dll"), "CreateServiceW");
#else
#define CreateService_T CreateServiceA_T
static api_fetcher<CreateService_T> CreateService_f(_T("Advapi32.dll"), "CreateServiceA");
#endif

//----------------------------------------------------------------------
//

BOOL bInstall;
SERVICE_STATUS_HANDLE hServiceStatus;
SERVICE_STATUS status;
DWORD dwThreadID;

TCHAR nt_service::service_name_[1024] = {0};
TCHAR nt_service::display_name_[MAX_PATH] = {0};
TCHAR nt_service::service_param_[50] = {0};

nt_service::nt_service(void)
{
    init();
}

nt_service::~nt_service(void)
{
}


//*********************************************************
//Functiopn:			init
//Description:			初始化
//Calls:				main
//Called By:				
//Table Accessed:				
//Table Updated:				
//Input:				
//Output:				
//Return:				
//Others:				
//History:				
//			<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
void nt_service::init()
{
    hServiceStatus = NULL;
    status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    status.dwCurrentState = SERVICE_STOPPED;
    status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    status.dwWin32ExitCode = 0;
    status.dwServiceSpecificExitCode = 0;
    status.dwCheckPoint = 0;
    status.dwWaitHint = 0;

    dwThreadID = GetCurrentThreadId();
}

//*********************************************************
//Functiopn:			ServiceMain
//Description:			服务主函数，这在里进行控制对服务控制的注册
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//			<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
void WINAPI nt_service::ServiceMain()
{
    // Register the control request handler
    status.dwCurrentState = SERVICE_START_PENDING;
	status.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	//注册服务控制
    hServiceStatus = RegisterServiceCtrlHandler(service_name_, ServiceStrl);
    if (hServiceStatus == NULL)
    {
        LogEvent(_T("Handler not installed"));
        return;
    }
    SetServiceStatus(hServiceStatus, &status);

    status.dwWin32ExitCode = S_OK;
    status.dwCheckPoint = 0;
    status.dwWaitHint = 0;
	status.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(hServiceStatus, &status);

	//模拟服务的运行，10后自动退出。应用时将主要任务放于此即可
	int i = 0;
	while (i < 10)
	{
		Sleep(1000);
		i++;
        Sleep(-1);
	}
	//

    status.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(hServiceStatus, &status);
    LogEvent(_T("Service stopped"));
}

//*********************************************************
//Functiopn:			ServiceStrl
//Description:			服务控制主函数，这里实现对服务的控制，
//						当在服务管理器上停止或其它操作时，将会运行此处代码
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:				dwOpcode：控制服务的状态
//Output:
//Return:
//Others:
//History:
//			<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
void WINAPI nt_service::ServiceStrl(DWORD dwOpcode)
{
    switch (dwOpcode)
    {
    case SERVICE_CONTROL_STOP:
		status.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(hServiceStatus, &status);
        PostThreadMessage(dwThreadID, WM_CLOSE, 0, 0);
        break;
    case SERVICE_CONTROL_PAUSE:
        break;
    case SERVICE_CONTROL_CONTINUE:
        break;
    case SERVICE_CONTROL_INTERROGATE:
        break;
    case SERVICE_CONTROL_SHUTDOWN:
        break;
    default:
        LogEvent(_T("Bad service request"));
    }
}

void nt_service::set_service_name(LPCTSTR service_name)
{
    memset(service_name_, 0, sizeof(service_name_));
    _tcsncpy(service_name_, service_name, _countof(service_name_));
}

void nt_service::set_display_name(LPCTSTR display_name)
{
    memset(display_name_, 0, sizeof(display_name_));
    _tcsncpy(display_name_, display_name, _countof(display_name_));
}

void nt_service::set_service_param(LPCTSTR service_param)
{
    memset(service_param_, 0, sizeof(service_param_));
    _tcsncpy(service_param_, service_param, _countof(service_param_));
}

//*********************************************************
//Functiopn:			is_installed
//Description:			判断服务是否已经被安装
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//			<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
BOOL nt_service::is_installed()
{
    BOOL bResult = FALSE;

	//打开服务控制管理器
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM != NULL)
    {
		//打开服务
        SC_HANDLE hService = ::OpenService(hSCM, service_name_, SERVICE_QUERY_CONFIG);
        if (hService != NULL)
        {
            bResult = TRUE;
            ::CloseServiceHandle(hService);
        }
        ::CloseServiceHandle(hSCM);
    }
    return bResult;
}

//*********************************************************
//Functiopn:			install
//Description:			安装服务函数
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//			<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
BOOL nt_service::install()
{
    if (is_installed())
        return TRUE;

    _ASSERT(display_name_[0] != _T('\0'));
    _ASSERT(service_name_[0] != _T('\0'));

	//打开服务控制管理器
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL)
    {
        MyPrtLog(_T("Install Couldn't open service manager: %s"), service_name_);
        return FALSE;
    }

    // Get the executable file path
    tstring str_path = sys_dir_helper::get_app_path();
    if (str_path.empty()) {
        return FALSE;
    }
    if (str_path[0] != _T('"')) {
        str_path.insert(str_path.begin(), _T('"'));
        str_path.append(_T("\""));
    }
    // 添加参数
    if (_tcslen(service_param_) > 0) {
        str_path.append(_T(" "));
        str_path.append(service_param_);
    }

    static CreateService_T CreateService_fun = (CreateService_T) CreateService_f;

	//创建服务
    SC_HANDLE hService = CreateService_fun (
        hSCM, service_name_, display_name_,
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
        str_path.c_str(), NULL, NULL, _T(""), NULL, NULL);

    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        MyPrtLog(_T("Install Couldn't create service"));
        return FALSE;
    }

    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
    return TRUE;
}

//*********************************************************
//Functiopn:			uninstall
//Description:			删除服务函数
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//			<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
BOOL nt_service::uninstall()
{
    if (!is_installed())
        return TRUE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM == NULL)
    {
        MyPrtLog(_T("Uninstall Couldn't open service manager"));
        return FALSE;
    }

    SC_HANDLE hService = ::OpenService(hSCM, service_name_, SERVICE_STOP | DELETE);

    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        MyPrtLog("Uninstall Couldn't open service");
        return FALSE;
    }
    SERVICE_STATUS status;
    ::ControlService(hService, SERVICE_CONTROL_STOP, &status);

	//删除服务
    BOOL bDelete = ::DeleteService(hService);
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);

    if (bDelete)
        return TRUE;

    LogEvent(_T("Service could not be deleted"));
    return FALSE;
}

void nt_service::start_run()
{
    SERVICE_TABLE_ENTRY st[] =
    {
        { service_name_, (LPSERVICE_MAIN_FUNCTION) ServiceMain },
        { NULL, NULL }
    };

    if (!::StartServiceCtrlDispatcher(st))
    {
        LogEvent(_T("Register Service Main Function Error!"));
    }
}

//*********************************************************
//Functiopn:			LogEvent
//Description:			记录服务事件
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//			<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
void nt_service::LogEvent(LPCTSTR pFormat, ...)
{
    TCHAR    chMsg[256];
    HANDLE  hEventSource;
    LPTSTR  lpszStrings[1];
    va_list pArg;

    va_start(pArg, pFormat);
    _vstprintf(chMsg, pFormat, pArg);
    va_end(pArg);

    lpszStrings[0] = chMsg;
	
	hEventSource = RegisterEventSource(NULL, service_name_);
	if (hEventSource != NULL)
	{
		ReportEvent(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, (LPCTSTR*) &lpszStrings[0], NULL);
		DeregisterEventSource(hEventSource);
	}
}


