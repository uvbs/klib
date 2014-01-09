#pragma once

/* winows ·þÎñÀà */
class nt_service
{
public:
    nt_service(void);
    ~nt_service(void);

protected:
    void init();

public:
    void set_service_name(LPCTSTR service_name);
    void set_display_name(LPCTSTR display_name);
    void set_service_param(LPCTSTR service_param);

    BOOL is_installed();
    BOOL install();
    BOOL uninstall();

    void start_run();

protected:
    static void LogEvent(LPCTSTR pszFormat, ...);
    static void WINAPI ServiceMain();
    static void WINAPI ServiceStrl(DWORD dwOpcode);
    
protected:
    static TCHAR service_name_[1024];
    static TCHAR display_name_[MAX_PATH];
    static TCHAR service_param_[50];
};

