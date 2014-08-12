#include <iostream>
#include "..\\yaolog.h"

#define LOGID_I "info"
#define LOGID_C "custom"
#define LOGID_B "bin"

int main()
{
    // call LogFactory::Init() at the app entry point
    YaoUtil::LogFactory::Init();

    // LOGID_I is a text logger
    // LOGID_C is a text logger
    // LOGID_B is a binary logger
    YaoUtil::LogFactory::Create(LOGID_I, true);
    YaoUtil::LogFactory::Create(LOGID_C, true);
    YaoUtil::LogFactory::Create(LOGID_B, true, false);

    LOGA__(LOGID_I, "My name is %s! I'm %d!", "neil", 29);

    // use default log file path
    // it is "module file dir\log\logID_time.log"
    YaoUtil::LogFactory::SetAttr(LOGID_I, true, 
        YaoUtil::LOGOUT_FLAG_STDOUT |
        YaoUtil::LOGOUT_FLAG_FILE,
        true, false, true, true);
    LOGW__(LOGID_I, L"My name is %s! I'm %d!", L"neil", 29);

    // set from ini file
    // if parameter ini_ is a filename like "logconfig.ini", 
    // then it must be in module file dir
    // or you can use an absolute path like "c:\logconfig.ini"
    YaoUtil::LogFactory::SetAttrFromConfigFile(LOGID_C, "logconfig.ini");
    for (int i = 0; i < 100; i++)
    {
        if (i == 80)
        {
            // disable all logs after this...
            YaoUtil::LogFactory::TotallyDisableAllLogs(true);
        }

        LOGA__(LOGID_C, "You can modify the ini file and view effect(%d)!", i);
        Sleep(500);
    }

    // cancel disable
    YaoUtil::LogFactory::TotallyDisableAllLogs(false);

    // output both to file and http server
    YaoUtil::LogFactory::SetAttr(LOGID_I, true, 
        YaoUtil::LOGOUT_FLAG_FILE |
        YaoUtil::LOGOUT_FLAG_REMOTE,
        true, true, true, true, false, "c:\\", "tt.log",
        "http://192.168.1.200/default.aspx");
    LOG__(LOGID_I, _T("My name is %s! I'm %d!"), _T("neil"), 29);

    // log binary data
    YaoUtil::LogFactory::SetAttrFromConfigFile(LOGID_B, "logconfig.ini");
    char buf[10] = { 0,1,2,3,4,5,6,7,8,9 };
    LOGBIN__(LOGID_B, buf, 10);

    std::cin.get();

    // call LogFactory::Exit() before app exit 
    YaoUtil::LogFactory::Exit();

    return 0;
}