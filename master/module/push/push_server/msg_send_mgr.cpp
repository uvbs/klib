#include "stdafx.h"
#include "msg_send_mgr.h"
#include <time.h>


const std::string g_PublicKey_N = "BA3F7EFF8A7AD3A6A445FD8C16E373F43C45A81D23E27588E13FCB8CA8A44F6E0E60AFD2E880759FF916A11025B1682A318FF0ADB3F45B5EE559E3D402A844B5DBEF71273D21AB8CE46F775964CE80617C645DF0994274041D993257A7F90BD50F0CDC84EA6AA31C766B2EB9A9832C0D13EE59EFF745B638AA0CCBAE6A939835";
const std::string g_PublicKey_E = "010001";
const std::string g_PublicKey_D = "2B20D8FFC3C97D897130E3EDB4BF26F2BFA241B3BDC38B040F64FE75ABF7A08D13B73FFFCEE73055C310826B82CEFA180B3211C1A79246364B55ED7402C9845F530C20F4A9B05FDE5327C4758A5C35F565C189D55CE87DB2DAE8DB19599948154519297A9E88510576BE1D50AEBF0B7225363F7BC7385C945574407AC5B25461";
const std::string g_PublicKey_P = "EB77AAC615FB26BEB7F1B6E4F875B0F96E883EF619C2C1B9D4E6F6361F17AC21DEAAED02939D94C581A1FC63636000A0250F3F620816C11776B9C62D4031360F";
const std::string g_PublicKey_Q = "CA7D1AFAD0F7AA355C0698F5B9A4C8178FB31A68A36E838A8286F315FC154D19EEF5532C6032AE4AF31B2D7FC5213E4C66291CADF0768D4624784D7306CE717B";
const std::string g_PublicKey_DP = "7332022C1288E2359FCBF30C25B97E312930228D47A10E2199D4BC5351F6C4D1751BEE03BAAFF71E8102FC74AB23466B1276592DDE833348055F7162A5EAED65";
const std::string g_PublicKey_DQ = "2754E9BAB2808905585CC7B5635C08A9DAF5D53962158363923B1858BAE0805B7B3917F0D4B6C37837849DEFE5A0B173319CE6BF3AA82C84E56CA0CEA911CCA5";
const std::string g_PublicKey_QP = "782CE20071098FAEC4204DDDDBAE1D981864484E686E53B42A0250FB56DB16600760A3A1345DB7B5E83006AEBB8AAAAB1389EA8F374A4B98C33B064EDA991A3E";

#pragma comment(lib, "sign_verify")

namespace logic
{

msg_send_mgr::msg_send_mgr():
    task_thread_pool_(2),
    max_retry_times_(DEFAULT_MAX_RETRY_TIMES),
    retry_send_interval(DEFAULT_RETRY_INTERVAL),
    whole_resend_times_(0)
{

    timer_mgr_.start();
    timer_mgr_.add(5*1000, std::bind(&msg_send_mgr::timer_check_resend_msg, this));

    // 签名信息填充
    sign_helper_.set_sign_info(g_PublicKey_N,
        g_PublicKey_E, 
        g_PublicKey_D, 
        g_PublicKey_P, 
        g_PublicKey_Q, 
        g_PublicKey_DP,
        g_PublicKey_DQ,
        g_PublicKey_QP);
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
        sign_helper_.sign_content((unsigned char*)msg->get_content().c_str(),
            msg->get_content().size(),
            str_sign);
        msg->set_str_sign(str_sign);
    }

    sign_on_send(uAddr, uPort, msg);

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

    sign_send_msg_success(itMsg->first, itMsg->second->get_user_msg());

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
        sign_send_msg_failed(itrConfirm->first, 
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
                sign_send_msg_failed(itMsg->first, itMsg->second->get_user_msg());

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
                    InterlockedIncrement64(&whole_resend_times_);

                    MyPrtLog(_T("重试发送消息:%I64d, 已发送次数%I64d"), 
                        pconfirm_info->get_msg_id(), 
                        pconfirm_info->get_sended_times());

                    sign_on_send(itMsg->first.get_addr(), 
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