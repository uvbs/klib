#ifndef _HTTP_REQUEST_
#define _HTTP_REQUEST_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "url.h"

using namespace std;

class HttpRequest {
	friend class HttpClient;
public:
	HttpRequest(const string &url = "", 
		        const string &method = "GET", int version = 1):
		m_url(url), m_method(method), m_httpversion(version) {
		initial();
	}
	
	HttpRequest(const char *url, 
                const char *method = "GET", int version = 1):
		m_url(url), m_method(method), m_httpversion(version) {
		initial();
	}	
		
	void set_method(const string &method) {
		m_method = method;
	}
	void set_method(const char *method) {
		m_method.assign(method);
	}
	
	void set_url(const string &url) {
		m_url.set_url(url);
	}
	void set_url(const char *url) {
		m_url.set_url(url);
	}
	
	void add_header(const string &name, const string &value) {
		m_headers[name] = value;
	}
	void add_header(const char *name, const char *value) {
		string n(name), v(value);
		m_headers[n] = v;
	}
	
	void set_body(const string &body) {
		m_body = body;
	}
	void set_body(const char *body) {
		string b(body);
		m_body.assign(body);
	}
	
	string get_header(const string &name);
	string get_header(const char *name);
	void   print_all(ostream &out);
	
	void  clear() {
		m_httpversion = 1;
		m_method = "GET";
		m_body = "";
		m_request = "";
		
	}
	void  reset(Url url) {
		clear();	
		m_url = url;
	}
	void  reset(const string &url) {
		clear();
		m_url.set_url(url);
	}
	void  reset(const char *url) {
		clear();
		m_url.set_url(url);
	}
	
private:
	static set<string>          m_method_set;
	static map<string, string>  m_default_header;
	
	void                        initial();
	bool                        make_request();
	
	Url                         m_url;	
	string                      m_method;
	int                         m_httpversion;	
	map<string, string>         m_headers;
	string                      m_body;
	string                      m_request;
};

#endif /* _HTTP_REQUEST_ */
