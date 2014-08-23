#ifndef _klib_stats_client_h_
#define _klib_stats_client_h_

#include <../../core/singleton.h>
using namespace klib;
using namespace klib::core;

#include <vector>
#include <map>
#include <string>

typedef std::pair<std::string, std::string>  param_pair_type;
typedef std::vector<param_pair_type> params_type;

// http提交的方式
enum e_http_method
{
    e_stats_get = 1,
    e_stats_pos,
    e_stats_default = e_stats_get,
};

// 统计客户端
class stats_client : public singleton<stats_client>
{
    stats_client(void);
public:
    ~stats_client(void);

    // url 是一个模板,里面的参数可以被替换
    void set_url(const std::string& url);
    void set_stats_method(e_http_method method);
    void add_get_param(const std::string& key, const std::string& val);
    void add_post_param(const std::string& key, const std::string& val);
    std::string exec();

protected:
    std::string  stats_url_;

    e_http_method  stats_method_;
    params_type  get_params_;
    params_type  post_params_;
};



#endif // _klib_stats_client_h_

