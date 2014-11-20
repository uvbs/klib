#include <iostream>
#include <string>
#include "tcp.h"
#include "url.h"
#include "httprequest.h"
#include "httpclient.h"

using namespace std;

/* test tcp client */
char buffer[1024];
void test_tcp()
{
	TcpClient tc;

	if (tc.connect_server("localhost", "80") != SUCCESS) {
		cout << "connect server failed!" << endl;
		return ;
	}
	
	string get("GET / HTTP/1.1\r\nHost: localhost\r\nAccept: text/*\r\nConnection: close\r\n\r\n");
	
	if (tc.writen(get.c_str(), get.size()) < 0) {
		cout << "send data failed!" << endl;
		return ;
	}
	if (tc.readn(buffer, 1024) < 0) {
		cout << "read data failed" << endl;
		return ;
	}
	
	cout << "revieve data:----------------->" << endl;
	cout << buffer << endl;
}
/*****************/

/* test url *
void test_url()
{
	Url url1("http://www.baidu.com/index.php");
	cout << "-----------sep----------" << endl;	
	Url url2("wwww.baidu.com/");
	cout << "-----------sep----------" << endl;	
	Url url3("baidu.com");
	cout << "-----------sep----------" << endl;	
	Url url4("https://qq.com");
	cout << "-----------sep----------" << endl;	
	Url url5("hellourl");	
	
}

/************/


/* test http request *
void test_http_request()
{
	HttpRequest hr("www.baidu.com");
	hr.print_all(cout);
}

/********************/

/* test http client */
void test_http_client()
{
	HttpClient hc;
	HttpRequest req("http://localhost/02.html");
	HttpResponse res;
	
	req.print_all(cout);
	
	hc.execute(req, res);
	hc.check_error(cout);
	res.print_all(cout);
}
/********************/

int main()
{
	/***
	test_tcp();
	/***/
	
	/***
	test_url();
	/***/
	
	/***
	test_http_request();
	/***/

	/***/
	test_http_client();
	/***/

	return 0;
}




