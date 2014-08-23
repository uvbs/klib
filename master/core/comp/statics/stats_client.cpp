#include "stats_client.h"



stats_client::stats_client(void)
    : stats_method_(e_stats_get)
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
    stats_method_ = method;
}

void stats_client::add_get_param(const std::string& key, const std::string& val)
{
    param_pair_type p;
    p.first  = key;
    p.second = val;
    get_params_.push_back(std::move(p));
}

void stats_client::add_post_param(const std::string& key, const std::string& val)
{
    param_pair_type p;
    p.first  = key;
    p.second = val;
    post_params_.push_back(std::move(p));
}

std::string stats_client::exec()
{
    return "";
}