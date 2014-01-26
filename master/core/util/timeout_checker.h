#ifndef _KLIB_TIMEOUT_CHECKER_H_
#define _KLIB_TIMEOUT_CHECKER_H_

#include <set>
#include <functional>
#include "../inttype.h"

namespace klib{
namespace util {


/*
 超时检测类
*/

typedef int64_t time_stamp_type ;

template <typename KeyType>
class timeout_checker
{
public:
    struct st_timeout_info
    {
        KeyType key_;
        time_stamp_type  time_stamp_;

        st_timeout_info() : time_stamp_(0) { }
        st_timeout_info(KeyType& key) : key_(key), time_stamp_(0) {}

        bool operator < (const st_timeout_info& other) const
        {
            return this->time_stamp_ < other.time_stamp_;
        }
    };

    typedef std::set<st_timeout_info> TimeoutSetType;
    typedef std::function<void(const KeyType& key)> CallBack;

    //------------------------------------------------------------------------
    // 判断是否为空
    bool empty() const 
    {
        return timeout_set_.empty();
    }
    
    //------------------------------------------------------------------------
    // 插入超时项
    time_stamp_type insert(KeyType key)
    {
        st_timeout_info tmout_info(key);
        tmout_info.time_stamp_ = GetTickCount64();

        auto itr_pair = timeout_set_.insert(tmout_info);
        while (!itr_pair.second) 
        {
            ++ tmout_info.time_stamp_;
            itr_pair = timeout_set_.insert(tmout_info);
        }

        return tmout_info.time_stamp_;
    }

    //------------------------------------------------------------------------
    // 通过插入时返回的超时时间戳可以删除超时项
    bool remove(time_stamp_type utime_stamp)
    {
        st_timeout_info key;
        key.time_stamp_ = utime_stamp;
        timeout_set_.erase(key);
        return true;
    }

    //------------------------------------------------------------------------
    // 超时时长:tmout 单位为毫秒
    void check(time_stamp_type tmout, const CallBack& fun)
    {
        if (timeout_set_.empty())  {  return;   }

        time_stamp_type utime_now = GetTickCount64();
        do 
        {
            // 清除超时的记录
            auto itr_tmout = timeout_set_.begin();
            if (itr_tmout->time_stamp_ + tmout < utime_now) 
            {
                auto key = itr_tmout->key_;
                timeout_set_.erase(itr_tmout);
                fun(key);
            }
            else
            {
                break;
            }

        } while (!timeout_set_.empty());
    }

    //------------------------------------------------------------------------
    // 检测是否是在检测的时限内
    bool is_limit(time_stamp_type check_interval) const 
    {
        static time_stamp_type ulast_check_time = 0;                         // 上次检测的时间
        time_stamp_type utime_now = GetTickCount64();                        // 当前系统运行时间
        
        // 判断是否需要进行超时检查
        time_stamp_type unext_check_time = ulast_check_time + check_interval;
        if (utime_now < unext_check_time) 
        {
            return true;
        }

        // 更新上次检测时间
        ulast_check_time = utime_now;

        return false;
    }

protected:
    std::set<st_timeout_info> timeout_set_;         ///< 保存超时信息

};


}}


#endif // _KLIB_TIMEOUT_CHECKER_H_