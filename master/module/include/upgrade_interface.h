#ifndef _klib_upgrade_interface_h_
#define _klib_upgrade_interface_h_


#include <istddef.h>
using namespace klib;

// {5DEB729F-7369-47ed-9177-50F54845898E}
static const GUID IID_UPGRADE_I = 
{ 0x5deb729f, 0x7369, 0x47ed, { 0x91, 0x77, 0x50, 0xf5, 0x48, 0x45, 0x89, 0x8e } };


// 
// define upgrade protocol

/*
关注点： 
    encrypt、urllocation、version

1、通过http请求获取升级信息

// 请求内容
http://xxx.com/api/?act=getver&ver=2.2.2.2&fro=soft&timestamp=133333

// 回复内容
<info>
    <ver>3.2.3.2580</ver>
    <hash>9B17F40D0444235DE0FF024F99F9CE5D6C134AAF</hash>
    <sign>....base64编码的字符串</sign>
    <forceupdate>true</forceupdate>
    <closeapp>true</closeapp>
    <desc></desc>
    <time></time>
    <sources>
        <source>
            <url>http://www.ldjflsfd.com/dfsdf.exe</url>
            <refer>if need</refer>
            <cookie>if need</cookie>
        </source>
    </sources>
</info>
*/

// upgrade
struct upgrade_info
{
public:
    std::string url_;
    std::string ver_;
};

//resource information
struct resource_info
{
    std::string url_;
    std::string cookie_;
    std::string refer_;
    std::string user_agent_;
};

// information of upgrade
struct version_info
{
    std::string version_;
    std::string hash_;           // md5或者sha1
    uint64_t    size_;           // file size
    std::string sign_;           // 签名（必须要验证）
    std::vector<resource_info> resoruces_;  // 资源
};

// event to upgrade
/*
enum upgrade_event
{
    upgrade_dwon_info_start,
    upgrade_down_info_success,
    upgrade_down_info_failed,

    upgrade_down_file_start,
    upgrade_down_file_success,
    upgrade_down_file_failed,

    upgrade_replace_file_start,
    upgrade_replace_file_success,
    upgrade_replace_file_failed,
};
typedef std::function<bool(upgrade_event) > callback;
*/

// upgrade interface
class upgrade_i
{
public:
    virtual bool set_cur_ver_info(const upgrade_info& loc) = 0;
    virtual bool get_new_ver_info(version_info& info) = 0;
    virtual bool down_new_file(const version_info& new_ver_info, 
        const std::string& new_path) = 0;

    virtual bool do_upgrade(const tstring& upgrade_pkg,
        const std::string& main_exe) = 0;

    virtual bool auto_upgrade() = 0;
};




#endif // _klib_upgrade_interface_h_