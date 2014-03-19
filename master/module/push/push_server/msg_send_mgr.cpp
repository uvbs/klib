#include "stdafx.h"
#include "msg_send_mgr.h"
#include <time.h>

#include "server_data.h"

namespace logic
{

msg_send_mgr::msg_send_mgr():
    max_retry_times_(DEFAULT_MAX_RETRY_TIMES),
    retry_send_interval(DEFAULT_RETRY_INTERVAL)
{
    timer_mgr_.start();
    timer_mgr_.add(5*1000, std::bind(&msg_send_mgr::timer_check_resend_msg, this));

    server_data::instance()->init();
}

msg_send_mgr::~msg_send_mgr(void)
{
}

void msg_send_mgr::set_max_retry_times(UINT32 uMaxRetryTimes)
{
    _ASSERT(uMaxRetryTimes > 0);
    max_retry_times_ = uMaxRetryTimes;
}

void msg_send_mgr::set_retry_send_interval(UINT32 uRetrySendInterval)
{
    _ASSERT(uRetrySendInterval > 0);
    retry_send_interval = uRetrySendInterval;
}

void msg_send_mgr::post_send_msg(ip_v4 uAddr, USHORT uPort, const push_msg_ptr msg)
{
    auto_lock lock(confirm_msg_mutex_);

    // 1.先找是否有该消息ID的待确认列表
    // 2.如果没有找到则直接添加该消息ID的相应信息
    auto itr = confirm_msg_map_.find(msg->get_msg_id());
    if (itr == confirm_msg_map_.end()) 
    {
        msg_confirm_info* pConfirmMsgInfo = confirm_msg_pool_.Alloc();
        if (NULL == pConfirmMsgInfo) {
            return;
        }
        pConfirmMsgInfo->set_user_msg(msg);

        pConfirmMsgInfo->set_msg_id(msg->get_msg_id());
        pConfirmMsgInfo->set_sended_times(1 + pConfirmMsgInfo->get_sended_times());
        pConfirmMsgInfo->set_last_send_time(_time64(NULL));

        confirm_msg_map_[msg->get_msg_id()][client_key(uAddr, uPort)] = pConfirmMsgInfo;
    }
    else
    {
        // 找到了该消息ID的，但未找到客户端的
        MapClientConfirmMsgInfoType& mapClientMsgMap = itr->second;
        auto itMsg = mapClientMsgMap.find(client_key(uAddr, uPort));
        if (itMsg == mapClientMsgMap.end()) 
        {
            msg_confirm_info* pConfirmMsgInfo = confirm_msg_pool_.Alloc();
            if (NULL == pConfirmMsgInfo) {
                return ;
            }
            pConfirmMsgInfo->set_user_msg(msg);

            pConfirmMsgInfo->set_msg_id(msg->get_msg_id());
            pConfirmMsgInfo->set_sended_times(1 + pConfirmMsgInfo->get_sended_times());
            pConfirmMsgInfo->set_last_send_time(_time64(NULL));

            mapClientMsgMap.insert(std::make_pair(client_key(uAddr, uPort), pConfirmMsgInfo));
        }
        else
        {
            // 找到相应的客户端了
            msg_confirm_info* pConfirmMsgInfo = itMsg->second;
            pConfirmMsgInfo->set_last_send_time(_time64(NULL));
            pConfirmMsgInfo->set_sended_times(1 + pConfirmMsgInfo->get_sended_times());
        }
    }

    // 对消息进行签名
    if (msg->get_str_sign().empty()) {
        std::string str_sign;
        server_data::instance()->get_sign_helper().
            sign_content((unsigned char*)msg->get_content().c_str(),
            msg->get_content().size(),
            str_sign);
        msg->set_str_sign(str_sign);
    }

    s_send_msg(uAddr, uPort, msg);

    return;
}

void msg_send_mgr::post_send_msg(ip_v4 uAddr, USHORT uPort, const std::vector<push_msg_ptr>& vecMsg)
{
    auto itr = vecMsg.begin();
    for (; itr != vecMsg.end(); ++ itr)
    {
        post_send_msg(uAddr, uPort, *itr);
    }
    return;
}

void msg_send_mgr::on_client_msg_ack(DWORD uAddr, USHORT uPort, UINT64 uMsgID)
{
    client_key key(uAddr, uPort);

    auto_lock lock(confirm_msg_mutex_);
    auto itr = confirm_msg_map_.find(uMsgID);
    if (itr == confirm_msg_map_.end()) {
        return;
    }

    MapClientConfirmMsgInfoType& client_confirm_msg_map = itr->second;
    auto itMsg = client_confirm_msg_map.find(key);
    if (itMsg == client_confirm_msg_map.end()) {
        return; 
    }

    s_on_send_msg_success(itMsg->first, itMsg->second->get_user_msg());

    // 在确认列表中清除该消息
    MyPrtLog(_T("收到客户端的确认消息..."));
    confirm_msg_pool_.Free(itMsg->second);
    client_confirm_msg_map.erase(itMsg);
}

BOOL msg_send_mgr::remove_msg_confirm_info(UINT64 uMsgID)
{
    auto_lock lock(confirm_msg_mutex_);

    auto itrMsgList = confirm_msg_map_.find(uMsgID);
    if (itrMsgList == confirm_msg_map_.end()) {
        return FALSE;
    }

    auto itrConfirm = itrMsgList->second.begin();
    for (; itrConfirm != itrMsgList->second.end(); ++ itrConfirm) 
    {
        s_on_send_msg_failed(itrConfirm->first, 
            itrConfirm->second->get_user_msg());

        confirm_msg_pool_.Free(itrConfirm->second);
    }
    itrMsgList->second.clear();
    confirm_msg_map_.erase(itrMsgList);

    return TRUE;
}

bool msg_send_mgr::timer_check_resend_msg()
{
    auto_lock lock(confirm_msg_mutex_);

    UINT64 uTimeNow = _time64(NULL);
    msg_confirm_info* pconfirm_info = NULL;
    auto itr = confirm_msg_map_.begin();
    for (; itr != confirm_msg_map_.end(); ++ itr)
    {
        auto itMsg = itr->second.begin();
        for (; itMsg != itr->second.end(); )
        {
            pconfirm_info = itMsg->second;

            // 删除超过最大发送次数的确认消息
            if (pconfirm_info->get_sended_times() >= max_retry_times_) 
            {
                s_on_send_msg_failed(itMsg->first, itMsg->second->get_user_msg());

                confirm_msg_pool_.Free(pconfirm_info);
                itMsg = itr->second.erase(itMsg);
                continue;
            }
            // 超过时间没有回复消息的，进行重试发送
            else 
            {
                if (pconfirm_info->get_last_send_time() + retry_send_interval < uTimeNow)
                {
                    pconfirm_info->set_last_send_time(uTimeNow);
                    pconfirm_info->set_sended_times(1 + pconfirm_info->get_sended_times());

                    MyPrtLog(_T("重试发送消息:%I64d, 已发送次数%I64d"), 
                        pconfirm_info->get_msg_id(), 
                        pconfirm_info->get_sended_times());

                    s_send_msg(itMsg->first.get_addr(), 
                        itMsg->first.get_port(), 
                        pconfirm_info->get_user_msg());
                }

                ++ itMsg;
            }
        }
    }

    return TRUE;
}



}