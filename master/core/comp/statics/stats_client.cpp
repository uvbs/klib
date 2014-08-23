#include "stats_client.h"


#pragma comment(lib, "ws2_32")

stats_client::stats_client(void)
{
}

stats_client::~stats_client(void)
{
}

void stats_client::set_url(const std::string& url)
{
    stats_url_ = url;
}

void stats_client::set_stats_method(e_http_method method)
{
    http_.set_stats_method(method);
}

void stats_client::add_get_param(const std::string& key, const std::string& val)
{
    get_params_.push_back(std::make_pair(key, val));
}

void stats_client::add_header_param(const std::string& key, const std::string& val)
{
    http_.add_header(key, val);
}

void stats_client::add_post_param(const std::string& key, const std::string& val)
{
    http_.add_post_param(key, val);
}

std::string stats_client::exec()
{
    std::string str_url = stats_url_;
    std::string str_content;

    if (str_url.find("?") == std::string::npos) {
        str_url.append("?");
    }
    else {
        str_url.append("&");
    }

    for (auto itr = get_params_.begin(); 
         itr != get_params_.end();
         itr ++)
    {
        str_url.append(itr->first);
        str_url.append("&");
        str_url.append(itr->second);
        str_url.append("=");
    }

    bool ret = http_.get_content(str_url, str_content);
    
    return std::move(str_content);
}