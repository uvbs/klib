#include "stdafx.h"
#include "upgrade_module.h"

#include <io/path.h>
#include <core/code_convert.h>
#include <net/http_down.h>
#include <util/strutil.h>
#include <encrypt/md5_file.h>
#include <core/process_helper.h>

#include <tinyxml.h>

BEGIN_REGISTER_MODULES
    REGISTER_MODULE(upgrade_module)
END_REGISTER_MODULES

const TCHAR* g_upgrade_help_exe = _T("updater.exe");

//----------------------------------------------------------------------
// @todo sign
upgrade_module::upgrade_module(void)
{
}

upgrade_module::~upgrade_module(void)
{
}

void upgrade_module::on_pre_run()
{
#ifdef _DEBUG

    version_info info;
    std::string str = ""
        "<info>"
            "<ver>3.2.3.2580</ver>"
            "<hash>9B17F40D0444235DE0FF024F99F9CE5D6C134AAF</hash>"
            "<sign>....base64编码的字符串</sign>"
            "<forceupdate>true</forceupdate>"
            "<closeapp>true</closeapp>"
            "<desc></desc>"
            "<time></time>"
            "<sources>"
                "<source>"
                    "<url>http://www.ldjflsfd.com/dfsdf.exe</url>"
                    "<refer></refer>"
                    "<cookie></cookie>"
                "</source>"
            "</sources>"
        "</info>"
        ;

    this->parser_upgrade_info(str.c_str(), info);

#endif

}

//----------------------------------------------------------------------
//
bool upgrade_module::set_upgrade_info(const upgrade_info& info)
{
    if (info.url_.empty() || 
        info.ver_.empty()) {
            return false;
    }

    upgrade_info_ = info;

    return true;
}

bool upgrade_module::get_new_ver_info(version_info& info)
{
    http_down downloader;

    std::string url;
    url = upgrade_info_.url_;
    if (url.empty()) {
        return false;
    }

    auto pos = url.find("?");
    if (pos == std::string::npos) {
        url += "?";
    }
    else {
        url += "&";
    }

    url += "ver=";
    url += upgrade_info_.ver_;

    url += "&from=soft";
    url += "&timestamp=";
    url += std::to_string(time(NULL));  // 时间戳有防止缓存的作用

    std::string ver_content;
    BOOL get_result = downloader.get_url_content(url.c_str(), ver_content);
    if (!get_result) 
        return false;

    // parser the information
    return parser_upgrade_info(ver_content, info);
}

bool upgrade_module::down_new_file(const version_info& new_ver_info, 
    const std::string& new_path)
{
    http_down downloader;

    for (auto itr=new_ver_info.resoruces_.begin(); 
        itr != new_ver_info.resoruces_.end(); 
        ++ itr)
    {
        std::string url = itr->url_;
        if (url.empty()) 
            continue;

        request_header_info info;
        info.cookie_ = itr->cookie_;
        info.refer_  = itr->refer_;
        info.user_agent_ = itr->user_agent_;
        BOOL down_result = downloader.download(url.c_str(), 
            new_path.c_str(),
            info);

        if (down_result) 
            return true;

        return false;
    }

    return false;
}

bool upgrade_module::do_upgrade(const tstring& upgrade_pkg,
    const std::string& main_exe) 
{
    // check package if is exists
    tstring cmd = upgrade_pkg;
    if (GetFileAttributes(upgrade_pkg.c_str()) == -1) 
        return false;

    // start updater to execute the update package
    klib::core::process_helper proc_helper;
    proc_helper.shell_execute(g_upgrade_help_exe, cmd.c_str());
    
    return true;
}

bool upgrade_module::auto_upgrade()
{
    // fetch information
    version_info ver_info;
    if (!get_new_ver_info(ver_info))
        return false;

    // download path
    klib::tstring local_path;
    klib::io::path::get_app_path(local_path);
    std::string local_p = klib::encode::code_convert::unicode_2_gbk(local_path);
    local_p += ver_info.version_;
    local_p += ".exe";
    if (!down_new_file(ver_info, local_p)) 
        return false;

    // verify
    local_path = klib::encode::code_convert::gbk_2_unicode(local_p);
    std::string md5;
    klib::encode::md5_file(md5, local_path.c_str());

    md5 = klib::util::toLower(md5);
    ver_info.hash_ = klib::util::toLower(ver_info.hash_);
    if (md5 != ver_info.hash_) 
        return false;

    // start updater to execute the update package
    klib::core::process_helper proc_helper;
    proc_helper.shell_execute(g_upgrade_help_exe, local_path.c_str());

    return true;
}

bool upgrade_module::parser_upgrade_info(const std::string& content, version_info& info)
{
    TiXmlDocument doc;
    
    doc.Parse(content.c_str());
    TiXmlElement* ele = doc.RootElement();
    if (nullptr == ele) {
        return false;
    }

    std::string name;
    std::string value;
    const char* txt = nullptr;
    TiXmlElement* child = ele->FirstChildElement();
    while (child)
    {
        name.clear();
        value.clear();

        if (txt = child->Value()) 
            name = txt;

        if (txt = child->GetText()) 
            value = txt;

        value = klib::util::trim(value);

        txt = child->Value();
        if (stricmp(txt, "ver") == 0) 
        {
            info.version_ = value;
        }
        else if (stricmp(txt, "hash") == 0) 
        {
            info.hash_ = value;
        }
        else if (stricmp(txt, "sign") == 0) 
        {
            info.sign_ = value;
        }
        else if (stricmp(txt, "forceupdate") == 0) 
        {
            if (value == "true") 
            {
                info.force_upgrade_ = true;
            }
            else if (value == "1") {
                info.force_upgrade_ = true;
            }
        }
        else if (stricmp(txt, "closeapp") == 0) 
        {
            if (value == "true") 
            {
                info.close_app_ = true;
            }
            else if (value == "1") {
                info.close_app_ = true;
            }
        }
        else if (stricmp(txt, "desc") == 0)
        {
            info.desc_ = value;
        }
        else if (stricmp(txt, "sources") == 0) 
        {
            TiXmlElement* sub_child = child->FirstChildElement();
            while (sub_child)
            {
                resource_info res;
                auto elet = sub_child->FirstChildElement("url");
                if (elet) {
                    res.url_ = elet->GetText();
                }

                elet = sub_child->FirstChildElement("refer");
                if (elet) {
                    res.refer_ = elet->GetText();
                    res.refer_ = klib::util::trim(res.refer_);
                }

                elet = sub_child->FirstChildElement("cookie");
                if (elet) {
                    res.cookie_ = elet->GetText();
                    res.cookie_ = klib::util::trim(res.cookie_);
                }

                info.resoruces_.push_back(std::move(res));
                sub_child = child->NextSiblingElement();
            }
        }
        
        child = child->NextSiblingElement();
    }

    return true;
}
