#include "stdafx.h"
#include "client_mgr.h"


namespace logic
{


client_mgr::client_mgr()
{
    timer_mgr_.start();
    timer_mgr_.add(10*1000, std::bind(&client_mgr::check_client_timeout, this));
}

client_mgr::~client_mgr(void)
{
}

client_info* client_mgr::update_client_info(client_key& key, PT_CMD_ONLINE& ptCmdOnline, BOOL& bNew)
{
    bNew = FALSE;
    int index = get_array_index(key.hash_key());
    auto_lock locker(client_info_mutex_[index]);

    auto itr = client_info_map_[index].find(key);
    if (itr == client_info_map_[index].end()) 
    {
        // 未找到，需要添加
        client_info* pInfo = client_info_pool_.Alloc();
        if (NULL == pInfo) 
            return NULL;

        // 保存客户端的信息数据
        if (!ptCmdOnline.channel.empty()) 
        {
            pInfo->login_name_ = ptCmdOnline.login_name.c_str();
            pInfo->mac_        = ptCmdOnline.mac.c_str();
            pInfo->channel_    = ptCmdOnline.channel.c_str();
        }

        // 客户端地址+端口
        pInfo->client_addr_ = key.get_addr();
        pInfo->client_port_ = key.get_port();

        // 保存版本值
        pInfo->version_     = ptCmdOnline.version;

        // 更新时间
        UINT64 uTimeNow     = _time64(NULL);
        pInfo->login_time_  = uTimeNow;
        pInfo->last_active_time_ = uTimeNow;

        // 更新心跳次数
        ++ pInfo->heart_count_;

        // 用户的帐号查看
        if (!ptCmdOnline.account.empty() || ptCmdOnline.uid != 0) 
        {
            pInfo->account_ = ptCmdOnline.account.c_str();
            pInfo->uid = ptCmdOnline.uid;

            sign_client_online.emit(pInfo);
        }

        // 保存信息
        client_info_map_[index].insert(std::make_pair(key, pInfo)) ;

        // 是新添加的
        bNew = TRUE;
        return pInfo;
    }
    else 
    {
        // 更新最后活跃时间
        itr->second->last_active_time_ = _time64(NULL);
        ++ itr->second->heart_count_;

        // 用户的帐号查看
        if (!ptCmdOnline.account.empty() || ptCmdOnline.uid != 0) 
        {
            client_info* pInfo = itr->second;

            // 比较一下如果信息不一致的话就要把新的信息更新
            if (ptCmdOnline.uid != pInfo->uid || 
                strcmp(ptCmdOnline.account.c_str(),  pInfo->account_.c_str()) != 0) 
            {
                // 发射退出信号通知
                sign_client_offline(pInfo);

                pInfo->account_ = ptCmdOnline.account.c_str();
                pInfo->uid = ptCmdOnline.uid;

                // 发射登陆信息号通知
                sign_client_online(pInfo);
            }
        }

        return itr->second;
    }
}

void client_mgr::broadcast_user_msg(push_msg_ptr pUserMsg, const std::string& channel)              // 广播消息
{
    // 投递到发送列表中去
    for (int i=0; i<bucket_size; ++i)
    {
        auto_lock lock(client_info_mutex_[i]);

        // 根据渠道判断是否需要发送消息
        auto itr = client_info_map_[i].begin();
        for (; itr != client_info_map_[i].end(); ++ itr)
        {
            if (channel.empty() ||
                _strnicmp(channel.c_str(), 
                itr->second->channel_.c_str(), 
                channel.size()) == 0) 
            {
                // 投递发送消息
                sign_send_push_msg(
                    itr->second->client_addr_, 
                    itr->second->client_port_,
                    pUserMsg
                    );

                MyPrtLog("投递广播消息:%s:%d", 
                    inet_ntoa(*(in_addr*)&itr->second->client_addr_), 
                    ntohs(itr->second->client_port_));
            }
        }
        //Sleep(10);
    }

    return ;
}

void client_mgr::for_each(std::function<void(client_info* pInfo)> fun)
{
    for (size_t i=0; i<bucket_size; ++i)
    {
        auto_lock lock(client_info_mutex_[i]);

        auto itr = client_info_map_[i].begin();
        for (; itr != client_info_map_[i].end(); ++itr)
        {
            fun(itr->second);
        }
    }
}

void  client_mgr::record_client_confirm_msg(DWORD uAddr, USHORT uPort, UINT64 uMsgID)
{
    client_key key(uAddr, uPort);

    int index = get_array_index(key.hash_key());
    auto_lock locker(client_info_mutex_[index]);

    client_info* pClientInfo;
    if (get_client_info(key, pClientInfo))
    {
        if (pClientInfo->confirm_lst_.is_full()) 
        {
            pClientInfo->confirm_lst_.pop_front();
        }
        pClientInfo->confirm_lst_.push_item(confirm_info(uMsgID, _time64(NULL)));
    }
}

BOOL client_mgr::is_client_exists(client_key& key)
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
    if (NULL == pszChannel) 
    {
        _ASSERT(FALSE) ;
        return 0;
    }

    UINT32 uChannelCount = 0;
    for (int i=0; i<bucket_size; ++i)
    {
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
    }

    return uChannelCount;
}

BOOL client_mgr::get_online_client_info(std::vector<client_info*>& vecClientInfo,
    UINT uStartItem, 
    UINT uFetchNum
    ) const
{
    UINT uIndex = 0;
    UINT uSkipCount = 0;
    BOOL bFinded = FALSE;
    for (; uIndex < bucket_size; ++ uIndex)
    {
        uSkipCount += client_info_map_[uIndex].size();
        if (uStartItem <= uSkipCount) 
        {
            bFinded = TRUE;
            uSkipCount -= client_info_map_[uIndex].size();
            break;
        }
    }

    UINT uNeedSkip = uStartItem - uSkipCount;
    UINT uFetchedNum = 0;           // 获取的个数
    client_info* pInfo = NULL;
    for (int i=uIndex; i<bucket_size; ++i)
    {
        auto itr = client_info_map_[i].begin();
        for (; itr != client_info_map_[i].end(); ++ itr)
        {
            // 需要跳过的个数
            if (uNeedSkip > 0) 
            {
                -- uNeedSkip;
                continue;
            }

            // 获取到足够的个数直接返回
            if (uFetchedNum >= uFetchNum) 
            {
                return TRUE;
            }

            // 申客户端信息内存
            pInfo = client_info_pool_.Alloc();
            if (NULL == pInfo) 
            {
                return TRUE;
            }

            // 返回到vector中
            * pInfo = *(itr->second);
            vecClientInfo.push_back(pInfo);

            ++ uFetchedNum;
        }
    }

    return TRUE;
}

void client_mgr::free_online_client_info(std::vector<client_info*>& vecClientInfo)
{
    auto itr = vecClientInfo.begin();
    for (; itr != vecClientInfo.end(); ++ itr)
    {
        client_info_pool_.Free(*itr);
    }
}

bool client_mgr::check_client_timeout()
{
    // 一段一段的检查超时的客户端
    UINT64 uTimeNow = NULL;
    for (int i=0; i<bucket_size; ++i)
    {
        if (TRUE)
        {
            auto_lock lock(client_info_mutex_[i]);

            uTimeNow = _time64(NULL);
            auto itr = client_info_map_[i].begin();
            for (; itr != client_info_map_[i].end(); )
            {
                // 超时了的释放到内存池中
                if (itr->second->last_active_time_ + CLIENT_DEFAULT_TIME_OUT < uTimeNow) 
                {
                    // 需先发射离线的信号
                    sign_client_offline.emit(itr->second);

                    // 再删除在线客户端的信息 
                    client_info_pool_.Free(itr->second);
                    itr = client_info_map_[i].erase(itr);
                }
                else
                {
                    ++ itr;
                }
            }

        }

        // 避免CPU负载高
        Sleep(50);
    }


    return TRUE;
}

BOOL client_mgr::get_client_info(const client_key& key, client_info*& pInfo)
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