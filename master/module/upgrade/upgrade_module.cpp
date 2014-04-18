#include "stdafx.h"
#include "upgrade_module.h"

#include <io/path.h>
#include <core/code_convert.h>
#include <net/http_down.h>


BEGIN_REGISTER_MODULES
    REGISTER_MODULE(upgrade_module)
END_REGISTER_MODULES

//----------------------------------------------------------------------
// 
upgrade_module::upgrade_module(void)
{
}

upgrade_module::~upgrade_module(void)
{
}

bool upgrade_module::set_cur_ver_info(const upgrade_info& info)
{
    upgrade_info_ = info;

    return true;
}

bool upgrade_module::get_new_ver_info(version_info& info)
{
    http_down downloader;

    std::string url;
    url = upgrade_info_.url_;
    url += "?ver";
    url += upgrade_info_.ver_;

    std::string ver_content;
    BOOL get_result = downloader.get_url_content(url.c_str(), ver_content);
    if (get_result) {
        return true;
    }

    return false;
}

bool upgrade_module::down_new_file(const version_info& new_ver_info, const std::string& new_path)
{
    http_down downloader;
       
    BOOL down_result = downloader.download(new_ver_info.download_url_.c_str(), 
        new_path.c_str());
    if (down_result) {
        return true;
    }

    return false;
}

bool upgrade_module::upgrade()
{
    version_info ver_info;
    if (!this->get_new_ver_info(ver_info))
    {
        return false;
    }

    // download path
    klib::tstring local_path;
    klib::io::path::get_app_path(local_path);
    std::string local_p = klib::encode::code_convert::unicode_2_gbk(local_path);
    local_p += ver_info.version_;
    local_p += ".exe";
    if (!this->down_new_file(ver_info, local_p)) 
    {
        return false;
    }

    // verify

    return true;
}

