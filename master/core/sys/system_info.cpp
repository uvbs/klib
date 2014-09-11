#include "system_info.h"


#include <windows.h>

using namespace klib::sys;

//读取操作系统的名称  
void system_info::get_system_name(std::string& osname)
{  
    SYSTEM_INFO info;        //用SYSTEM_INFO结构判断64位AMD处理器   
    GetSystemInfo(&info);    //调用GetSystemInfo函数填充结构   
    OSVERSIONINFOEX os;   
    os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);     

    osname = ("unknown OperatingSystem.");  

    if(GetVersionEx((OSVERSIONINFO *)&os))  
    {   
        //下面根据版本信息判断操作系统名称   
        switch(os.dwMajorVersion)//判断主版本号  
        {  
        case 4:  
            switch(os.dwMinorVersion)//判断次版本号   
            {   
            case 0:  
                if(os.dwPlatformId==VER_PLATFORM_WIN32_NT)  
                    osname = ("Microsoft Windows NT 4.0"); //1996年7月发布   
                else if(os.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS)  
                    osname = ("Microsoft Windows 95");  
                break;  
            case 10:  
                osname = ("Microsoft Windows 98");  
                break;  
            case 90:  
                osname = ("Microsoft Windows Me");  
                break;  
            }  
            break;  

        case 5:  
            switch(os.dwMinorVersion)   //再比较dwMinorVersion的值  
            {   
            case 0:  
                osname = ("Microsoft Windows 2000");//1999年12月发布  
                break;  

            case 1:  
                osname = ("Microsoft Windows XP");//2001年8月发布  
                break;  

            case 2:  
                if(os.wProductType==VER_NT_WORKSTATION   
                    && info.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)  
                {  
                    osname =_T("Microsoft Windows XP Professional x64 Edition");  
                }  
                else if(GetSystemMetrics(SM_SERVERR2)==0)  
                    osname =_T("Microsoft Windows Server 2003");//2003年3月发布   
                else if(GetSystemMetrics(SM_SERVERR2)!=0)  
                    osname =_T("Microsoft Windows Server 2003 R2");  
                break;  
            }  
            break;  

        case 6:  
            switch(os.dwMinorVersion)  
            {  
            case 0:  
                if(os.wProductType == VER_NT_WORKSTATION)  
                    osname = ("Microsoft Windows Vista");  
                else  
                    osname = ("Microsoft Windows Server 2008");//服务器版本   
                break;  
            case 1:  
                if(os.wProductType == VER_NT_WORKSTATION)  
                    osname = ("Microsoft Windows 7");  
                else  
                    osname = ("Microsoft Windows Server 2008 R2");  
                break;  
            }  
            break;  
        }  
    }//if(GetVersionEx((OSVERSIONINFO *)&os))  

}  

//读取操作系统的版本名称  
void system_info::get_version_mark(std::string& vmark)
{
    OSVERSIONINFOEX os; 
    os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    vmark.clear();

    if(GetVersionEx((OSVERSIONINFO*)&os))  
    {   
        switch(os.dwMajorVersion)  
        {                //先判断操作系统版本   
        case 5:   
            switch(os.dwMinorVersion)  
            {   
            case 0:                  //Windows 2000   
                if(os.wSuiteMask==VER_SUITE_ENTERPRISE)
                {
                    vmark = ("Advanced Server");
                }
                break;   
            case 1:                  //Windows XP   
                if(os.wSuiteMask==VER_SUITE_EMBEDDEDNT) 
                {
                    vmark = ("Embedded");
                }
                else if(os.wSuiteMask==VER_SUITE_PERSONAL)
                {
                    vmark = ("Home Edition");
                }
                else 
                {
                    vmark = ("Professional");
                }
                break;
            case 2:
                if(GetSystemMetrics(SM_SERVERR2)==0
                    &&os.wSuiteMask==VER_SUITE_BLADE)  //Windows Server 2003 
                {
                    vmark = ("Web Edition");
                }
                else if(GetSystemMetrics(SM_SERVERR2)==0
                    && os.wSuiteMask==VER_SUITE_COMPUTE_SERVER)
                {
                    vmark = ("Compute Cluster Edition");
                }
                else if(GetSystemMetrics(SM_SERVERR2)==0
                    && os.wSuiteMask==VER_SUITE_STORAGE_SERVER)
                {
                    vmark = ("Storage Server");
                }
                else if(GetSystemMetrics(SM_SERVERR2) == 0 
                    && os.wSuiteMask==VER_SUITE_DATACENTER)
                {
                    vmark = ("Datacenter Edition");
                }
                else if(GetSystemMetrics(SM_SERVERR2) == 0
                    && os.wSuiteMask==VER_SUITE_ENTERPRISE)
                {
                    vmark = ("Enterprise Edition");
                }
                else if(GetSystemMetrics(SM_SERVERR2) != 0
                    && os.wSuiteMask==VER_SUITE_STORAGE_SERVER)
                {
                    vmark = ("Storage Server");
                }
                break;
            }
            break;

        case 6:  
            switch(os.dwMinorVersion)
            {
            case 0:
                if(os.wProductType!=VER_NT_WORKSTATION
                    && os.wSuiteMask==VER_SUITE_DATACENTER)
                {
                    vmark = ("Datacenter Server");
                }
                else if(os.wProductType!=VER_NT_WORKSTATION
                    && os.wSuiteMask==VER_SUITE_ENTERPRISE)
                {
                    vmark = ("Enterprise");
                }
                else if(os.wProductType==VER_NT_WORKSTATION 
                    && os.wSuiteMask==VER_SUITE_PERSONAL)  //Windows Vista  
                {
                    vmark = ("Home");
                }
                break;
            }
            break;
        }
    }
}