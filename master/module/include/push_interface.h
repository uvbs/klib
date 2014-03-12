#ifndef _push_framework_interface_h_
#define _push_framework_interface_h_

// {5DBCA606-57CA-4d08-96A3-2A117550DB85}
static const GUID IID_PUSH_CLIENT = 
{ 0x5dbca606, 0x57ca, 0x4d08, { 0x96, 0xa3, 0x2a, 0x11, 0x75, 0x50, 0xdb, 0x85 } };

// {DC52BBBC-45C7-44fd-86F0-C8C00C21F7EB}
static const GUID IID_PUSH_SERVER = 
{ 0xdc52bbbc, 0x45c7, 0x44fd, { 0x86, 0xf0, 0xc8, 0xc0, 0xc, 0x21, 0xf7, 0xeb } };


#include <net/udp_client.h>
#include <functional>

class push_msg
{
public:
    push_msg() : msg_id_(0), msg_type_(0), delay_fetch_(0), delay_show_(0) {}

    UINT64           msg_id_;             ///< 消息编号
    UINT32           msg_type_;           ///< 消息类型
    UINT32           show_time_;          ///< 显示的时间（即要显示多长时间）
    UINT32           delay_fetch_;        ///< 延迟多长时间获取消息
    UINT32           delay_show_;         ///< 显示延迟时间（即指需要延迟多少时间显示）
    std::string      str_sign_;           ///< 消息内容的签名（计算出内容的md5值,使用签名算法签名）
    std::string      content_;            ///< 消息内容(xml内容)
};
typedef std::shared_ptr<push_msg> push_msg_ptr;

// 处理数据回调
typedef std::function<void(udp_client*client_, char* buff, size_t len)> 
    handle_data_callback;

// 处理消息回调
typedef std::function<void(push_msg_ptr)> 
    handle_msg_callback;

enum push_client_status
{
    status_connecting,
    status_query_logic_addr,
    status_query_newver,
    status_online,
};
class push_client_i
{
    virtual bool set_option(std::string& domain, USHORT uport) = 0;
    virtual bool set_data_callback(handle_data_callback& callback) = 0;
    virtual bool set_msg_callback(handle_msg_callback& callback) = 0;
    virtual bool set_client_info(const std::string& channel,
        const std::string& mac,
        const std::string& user,
        const std::string& pass,
        const std::string& last_msg_id, 
        const std::string& appid) = 0;

    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual push_client_status get_status() = 0;
};

class push_server_i
{

};


#endif // _push_framework_interface_h_