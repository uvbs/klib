

#include <net/http/http.h>
#include <net/winsock_init.h>
using namespace klib::net;

#include <util/strutil.h>
using namespace klib::util;

#include <encrypt/base64.h>
using namespace klib::encode;

#include <comp/statics/stats_client.h>
#include <comp/statics/mac_extractor.h>


winsock_init  g_winsock_initor;

TEST(http, 1)
{
#define  form_str "{random}*Yes,pca*{mac},pod*{pid},uod*{uid},ip*{ip},os*winxp,poison*1_2_3_4,list*d:/asdasd/asds,status*1"
    std::string str_format = form_str;

    mac_extractor mac_;
    mac_.extract_mac_lst(e_extract_helpapi);
    std::string str_mac = mac_.get_mac_list().front().mac_;

    str_format = Replace(str_format, "{random}", "203984");
    str_format = Replace(str_format, "{mac}", str_mac);
    str_format = Replace(str_format, "{pid}", "23");
    str_format = Replace(str_format, "{uid}", "23");
    str_format = Replace(str_format, "{ip}",  "127.0.0.1");

    base64 b64;
    b64.encode(str_format.c_str(), str_format.size(), str_format, false);

    str_format = Replace(str_format, "==",  "QegA27vc");
    str_format = Replace(str_format, "=",  "QegA27vcA");
    str_format = Replace(str_format, "O",  "IkL");
    str_format = Replace(str_format, "R",  "TgF");
    str_format = Replace(str_format, "A",  "SwD");

    /*
    ==  Ìæ»»  QegA27vc
    =   Ìæ»»  QegA27vcA
    O   Ìæ»»  IkL
    R   Ìæ»»  TgF
    A   Ìæ»»  SwD
    */

    stats_client* client = stats_client::instance();
    client->set_stats_method(e_stats_post);
    client->set_url("http://inapi.91ox.net/index.php?");

    client->add_get_param("encode", str_format);
    client->add_get_param("process", "kk.exe");

    client->exec();
}