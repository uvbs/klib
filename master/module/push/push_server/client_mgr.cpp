#include "stdafx.h"
#include "client_mgr.h"


namespace logic
{


client_mgr::client_mgr()
{
    timer_mgr_.start();
    timer_mgr_.add(10*1000, std::bind(&client_mgr::tiner_check_client_timeout, this));
}

client_mgr::~client_mgr(void)
{
}

client_info_ex* client_mgr::update_client_info(client_addr_key& key, PT_CMD_ONLINE& cmd_online, BOOL& bNew)
{
    bNew = FALSE;
    int index = get_array_index(key.hash_key());
    auto_lock locker(client_info_mutex_[index]);
    client_info_ex* pInfo = nullptr;

    auto itr = client_info_map_[index].find(key);
    if (itr == client_info_map_[index].end()) 
    {
        // 未找到，需要添加
        pInfo = client_info_pool_.Alloc();
        if (nullptr == pInfo) 
            return nullptr;

        bNew = TRUE;  // 是新添加的

        // 保存客户端的信息数据
        pInfo->login_name_  = cmd_online.login_name.c_str();
        pInfo->mac_         = cmd_online.mac.c_str();
        pInfo->channel_     = cmd_online.channel.c_str();
        pInfo->client_addr_ = key.get_addr();       // 客户端地址+端口
        pInfo->client_port_ = key.get_port();
        pInfo->version_     = cmd_online.version;   // 保存版本值
        pInfo->login_time_  = _time64(NULL);        // 更新时间 
        pInfo->last_active_time_ = pInfo->login_time_;
        pInfo->heart_count_ = 1;                    // 更新心跳次数
        pInfo->account_ = cmd_online.account.c_str();// 用户的帐号查看
        pInfo->uid      = cmd_online.uid;

        if (s_on_client_online)
            s_on_client_online(pInfo);

        // 保存信息
        client_info_map_[index].insert(std::make_pair(key, pInfo)) ;
        return pInfo;
    }
    else 
    {
        pInfo = itr->second;
        pInfo->last_active_time_ = _time64(NULL);
        pInfo->heart_count_ ++;

        // 判断信息是否改变了
        if (cmd_online.uid != pInfo->uid || 
            strcmp(pInfo->account_.c_str(), cmd_online.account.c_str()) != 0) 
        {
            if (s_on_client_offline)
                s_on_client_offline(pInfo);     // 发射退出信号通知

            pInfo->set_login_time(_time64(NULL));
            pInfo->set_last_active_time(pInfo->get_last_active_time());            
            pInfo->account_ = cmd_online.account.c_str();
            pInfo->uid      = cmd_online.uid;

            if (s_on_client_online)
                s_on_client_online(pInfo);      // 发射登陆信息号通知
        }

        return itr->second;
    }
}

void client_mgr::broadcast_user_msg(push_msg_ptr pUserMsg, const std::string& channel)              // 广播消息
{
    for (int i=0; i<bucket_size; ++i)
    {
        auto_lock lock(client_info_mutex_[i]);

        auto itr = client_info_map_[i].begin();
        for (; itr != client_info_map_[i].end(); ++ itr)
        {
            if (channel.empty() || 
                strcmp(channel.c_str(), itr->second->channel_.c_str()) == 0) 
            {
                s_on_post_push_msg(
                    itr->second->client_addr_, 
                    itr->second->client_port_,
                    pUserMsg
                );
            }
        }
    }

    return ;
}

void  client_mgr::add_client_confirm_msg_id(ip_v4 uAddr, USHORT uPort, UINT64 uMsgID)
{
    client_addr_key key(uAddr, uPort);

    int index = get_array_index(key.hash_key());
    auto_lock locker(client_info_mutex_[index]);

    client_info_ex* c_info;
    if (get_client_info(key, c_info))
    {
        if (c_info->confirm_lst_.is_full()) 
            c_info->confirm_lst_.pop_front();

        c_info->confirm_lst_.push_item(confirm_info(uMsgID, _time64(NULL)));
    }
}

BOOL client_mgr::is_client_exists(client_addr_key& key)
{
    int index = get_array_index(key.hash_key());
    auto_lock locker(client_info_mutex_[index]);

    auto itr = client_info_map_[index].find(key);
    if (itr == client_info_map_[index].end()) {
        return FALSE;
    }

    return TRUE;
}

UINT32 client_mgr::get_online_client_count() const
{
    UINT32 uExistClient = 0;
    for (int i=0; i<bucket_size; ++i)
    {
        uExistClient += client_info_map_[i].size();
    }
    return uExistClient;
}

UINT32 client_mgr::get_channel_count(const char* pszChannel) 
{
    if (nullptr == pszChannel) {
        _ASSERT(FALSE) ;
        return 0;
    }

    UINT32 uChannelCount = 0;
    for (int i=0; i<bucket_size; ++i)
    {
        auto_lock lock(client_info_mutex_[i]);

        auto itr = client_info_map_[i].begin();
        for (; itr != client_info_map_[i].end(); ++ itr)
        {
            if (strcmp(itr->second->channel_.c_str(), pszChannel) == 0)
            {
                ++ uChannelCount;
            }
        }
    }

    return uChannelCount;
}

BOOL client_mgr::get_online_client_info(std::vector<client_info_ex*>& vecClientInfo,
    UINT uStartItem, 
    UINT uFetchNum
    ) const
{
    UINT uIndex = 0;
    UINT uSkipCount = 0;
    for (; uIndex < bucket_size; ++ uIndex)
    {
        uSkipCount += client_info_map_[uIndex].size();
        if (uStartItem <= uSkipCount) 
        {
            uSkipCount -= client_info_map_[uIndex].size();
            break;
        }
    }

    UINT need_skip_num = uStartItem - uSkipCount;
    UINT already_feteched_num = 0;           // 获取的个数
    client_info_ex* pInfo = nullptr;
    for (int i=uIndex; i<bucket_size; ++i)
    {
        auto itr = client_info_map_[i].begin();
        for (; itr != client_info_map_[i].end() && already_feteched_num < uFetchNum; 
              ++ itr)
        {
            // 需要跳过的个数
            if (need_skip_num > 0) {
                -- need_skip_num;
                continue;
            }
            
            // 申客户端信息内存
            pInfo = client_info_pool_.Alloc();
            if (nullptr == pInfo) 
                return TRUE;

            // 返回到vector中
            * pInfo = *(itr->second);
            vecClientInfo.push_back(pInfo);
            ++ already_feteched_num;
        }
    }

    return TRUE;
}

void client_mgr::free_online_client_info(std::vector<client_info_ex*>& vecClientInfo)
{
    auto itr = vecClientInfo.begin();
    for (; itr != vecClientInfo.end(); ++ itr)
    {
        client_info_pool_.Free(*itr);
    }
}

bool client_mgr::tiner_check_client_timeout()
{
    // 一段一段的检查超时的客户端
    UINT64 uTimeNow = 0;
    for (int i=0; i<bucket_size; ++i)
    {
        auto_lock lock(client_info_mutex_[i]);

        uTimeNow = _time64(NULL);
        auto itr = client_info_map_[i].begin();
        for (; itr != client_info_map_[i].end(); )
        {
            // 超时了的释放到内存池中
            if (itr->second->last_active_time_ + client_default_timeout < uTimeNow) 
            {
                // 需先发射离线的信号
                if (s_on_client_offline)
                    s_on_client_offline(itr->second);

                // 再删除在线客户端的信息 
                client_info_pool_.Free(itr->second);
                itr = client_info_map_[i].erase(itr);
            }
            else
            {
                ++ itr;
            }
        }

        // 避免CPU负载高
        Sleep(50);
    }
    
    return TRUE;
}

BOOL client_mgr::get_client_info(const client_addr_key& key, client_info_ex*& pInfo)
{
    int index = get_array_index(key.hash_key());

    auto itr = client_info_map_[index].find(key);
    if (itr == client_info_map_[index].end()) {
        return FALSE;
    }

    pInfo = & *itr->second;

    return TRUE;
}


}