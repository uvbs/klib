

#include <net/http/http.h>
#include <net/winsock_init.h>

#include <comp/statics/stats_client.h>

using namespace klib::net;

winsock_init  g_winsock_initor;

TEST(http, 1)
{
    stats_client* client = stats_client::instance();
    client->set_stats_method(e_stats_post);
    client->set_url("http://inapi.91ox.net/index.php?encode=encode&process=pp");
    //client->set_url("http://www.baidu.com/index.php?encode=encode&process=pp");
    client->exec();


}