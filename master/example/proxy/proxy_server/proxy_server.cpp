// proxy_server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "proxy_server_t.h"
bool extract_content(const std::string& str, std::string& content);

std::string test_str = "\
GET http://www.baidu.com/home/hot/data/wordtips?word=%E6%B5%81%E6%B5%AA%E6%B1%89%E4%B9%B0%E5%BD%A9%E7%A5%A8%E4%B8%AD1720%E4%B8%87%E5%A4%A7%E5%A5%96&_t=1393854856830 HTTP/1.1\r\n\
Host: www.baidu.com\r\n\
Proxy-Connection: keep-alive\r\n\
User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/33.0.1750.117 Safari/537.36\r\n\
X-Requested-With: XMLHttpRequest\r\n\
Accept: */*\r\n\
DNT: 1\r\n\
Referer: http://www.baidu.com/\r\n\
Accept-Encoding: gzip,deflate,sdch\r\n\
Accept-Language: en-US,en;q=0.8,ja;q=0.6,pt;q=0.4,zh-CN;q=0.2,zh;q=0.2,zh-TW;q=0.2\r\n\
Cookie: bdshare_firstime=1376237660110; BDRCVFR[JZXeEuSo-Hf]=I67x6TjHwwYf0; BDRCVFR[feWj1Vr5u3D]=I67x6TjHwwYf0; BDRCVFR[S4-dAuiWMmn]=I67x6TjHwwYf0; shifen[1440961942_7206]=1377261971; shifen[1751526531_49522]=1377262309; shifen[2973774674_76113]=1377262311; shifen[54233643_61028]=1377509867; shifen[2917870732_87961]=1378044758; shifen[584627060_13453]=1379688011; shifen[1632820739_83416]=1379725884; shifen[5190149305_95749]=1379725889; BAIDUID=82FCB19CC9E3D7B9957E207099683A22:FG=1; shifen[45952255_34919]=1382628320; locale=zh; shifen[3144900788_20085]=1384259884; H_PS_TIPCOUNT=5; H_PS_TIPFLAG=; shifen[93295689_17432]=1386077949; BDUSS=0hWR2VUS0RBenFleEQwem5WNG5IdXFKcWJUM3VDU3RoN0tzZ0tJQzlYNzgyOFpTQVFBQUFBJCQAAAAAAAAAAAEAAAC42H8DZG9uZ2t1bmRqeQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAPxOn1L8Tp9SM; BAIDU_DUP_lcr=https://www.google.com.hk/; Hm_lvt_9f14aaa038bbba8b12ec2a4a3e51d254=1390663309; Hm_lpvt_9f14aaa038bbba8b12ec2a4a3e51d254=1392453111; BDSFRCVID=GetsJeCCxG00GCQCpvPeQyO6wpMAhrAsu_sx3J; H_BDCLCKID_SF=JR-eoCPhJIvbfP0kbPQoKPkthf5KetJyaR3M345bWJ5TMCoq047dhTtdLfcC3xue-DCOWlkMfU3jShPC-tnGbRKihxTE-JjGW2Q9BbOj3l02V-bIe-t2ynQD0JnO-tRMW23v0h7mWP05VKFCj50Be5Jyep5-K-cebITHstnVKRr8jJjkq4bohjnXX-reBtQmJJvxb-cm5ROTfn7l547i3t6QbtRuXfJNQg-q3xt5HR5Mhtc-BPOF0hKRKfTi0x-jLTnhVn0MW-KVHfn4b4nJyUnQbPnnBTKtLnLD_KIMfII-bP365ITD-tFO5eT22-us3KTJQhcH0hOWsItxQUbGLn8B0MoIhx4qb5Ru5Rn-04Oh_PO9DUC0-nDSHHKjJ50q3J; BDREFER=%7Burl%3A%22http%3A//lady.baidu.com/%22%2Cword%3A%22%22%7D; BD_CK_SAM=1; H_PS_PSSID=5413_1422_5138_5225_5367_5379_5368_4261_4760_5405\r\n\
\r\n\
\r\n";

int _tmain(int argc, _TCHAR* argv[])
{
    std::string str_content;
    extract_content(test_str, str_content);

     system("color 03");

    proxy_server_t* p = proxy_server_t::instance();
    p->start();

    Sleep(-1);
	return 0;
}

