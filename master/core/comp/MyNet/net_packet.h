#ifndef _klib_net_packet_h
#define _klib_net_packet_h

#include "net_conn.h"
class tcp_net_facade_imp;

// 保存网络数据包的
template<size_t t_fix_buf_size>
class net_packet_t
{
    friend tcp_net_facade_imp;
public:
    net_packet_t(void)
    {
        buff_size_        = 0;
        buff_ptr_         = NULL;
    }

    ~net_packet_t(void)
    {
        if (buff_size_ > t_fix_buf_size) {
            delete [] buff_ptr_;
        }
    }

    bool init_buff(size_t bf_size)
    {
        if (bf_size < t_fix_buf_size) {
            buff_ptr_  = fix_buff_;
            buff_size_ = bf_size;
            return true;
        }

        delete [] buff_ptr_;

        buff_size_ = bf_size;
        buff_ptr_  = new char[bf_size];
        return (NULL != buff_ptr_);
    }

    char* get_buff() 
    {
        return buff_ptr_;
    }

protected:
    std::weak_ptr<net_conn>   conn_;	                            // 属于哪个连接

    size_t      buff_size_;                         // 缓存区中保存数据的大小
    char*       buff_ptr_;                          // 缓冲区
    char        fix_buff_[t_fix_buf_size];               // 固定缓冲区
};

typedef net_packet_t<1024> net_packet;

#endif
