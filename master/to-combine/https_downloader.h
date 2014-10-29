#ifndef _CSDCS_HTTPS_DOWNLOADER_H
#define _CSDCS_HTTPS_DOWNLOADER_H


// 失败原因定义
enum e_failed_reason 
{
    e_failed_success,
    e_failed_create_file,
    e_failed_handshake,
    e_failed_connect,
    e_failed_write,
    e_failed_response,
    e_failed_download,
    e_failed_unknow
};

// https下载器
class https_downloader
{
    https_downloader(const https_downloader&);
    https_downloader& operator = (const https_downloader&);

public:
    ~https_downloader(void);
    https_downloader();
    
    bool download(const std::string& url, const std::string& path);

};


#endif  // _CSDCS_HTTPS_DOWNLOADER_H
