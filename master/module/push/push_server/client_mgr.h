#pragma once


#include "client_info.h"
#include "msg_send_mgr.h"
#include "../common/protocol_st.h"


namespace logic
{

//----------------------------------------------------------------------
// 客户端管理类
class client_mgr
{
public:
    client_mgr();
    ~client_mgr(void);

#define     bucket_size                 (29)            ///< 定义hash桶的大小
#define     CLIENT_DEFAULT_TIME_OUT     (150)           ///< 默认为150秒超时

public:
    //----------------------------------------------------------------------
    // 信号
    sigslot::signal1<client_info*>      sign_client_online;
    sigslot::signal1<client_info*>      sign_client_offline;
    sigslot::signal3<ip_v4, USHORT, const push_msg_ptr> sign_send_push_msg;

public:
    //----------------------------------------------------------------------
    // key  :  作为查找客户端的一个键值
    // bNew :  返回标记（是否是一个新的客户端）
    client_info* update_client_info(client_key& key, PT_CMD_ONLINE& ptCmdOnline, BOOL& bNew);        ///< 更新客户端信息
    void broadcast_user_msg(push_msg_ptr pUserMsg, const std::string& channel);                      ///< 广播消息
    void for_each(std::function<void(client_info* pInfo)> fun);

    void record_client_confirm_msg(DWORD uAddr, USHORT uPort, UINT64 uMsgID);          ///< 记录客户端的回应消息记录
    BOOL is_client_exists(client_key& key);                                            ///< 判断客户端是否在线

    UINT32 get_online_client_count() const;                                            ///< 获得客户端的个数
    UINT32 get_channel_count(const char* pszChannel) ;                                 ///< 获取指定渠道的客户端个数
    BOOL   get_online_client_info(std::vector<client_info*>& vecClientInfo,            ///< 获取客户信息列表(获取之后要调用free_online_client_info释放)
        UINT uStartItem, 
        UINT uFetchNum
        ) const;
    void   free_online_client_info(std::vector<client_info*>& vecClientInfo);

protected:
    bool check_client_timeout();                                                    ///< 检查客户端是否超时（即一段时间内没有心跳数据）
    BOOL get_client_info(const client_key& key, client_info*& pInfo);
    inline int get_array_index(UINT uKey) {  return uKey % bucket_size;  }          ///< 通过hash获取数组的索引

protected:
    // 作一个hash并发访问的数组
    typedef std::map<client_key, client_info*>  mapClientIntoType;
    mapClientIntoType                           client_info_map_[bucket_size];      ///< 保存客户端信息的数组
    auto_cs                                     client_info_mutex_[bucket_size];    ///< 客户端信息互斥对象

    mutable CObjectPool<client_info,10000, 1000>  client_info_pool_;                ///< 定时器模块
    timer_mgr                                     timer_mgr_;
};


}