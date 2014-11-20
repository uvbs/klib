#ifndef _URL_H_
#define _URL_H_

#include <string>
using namespace std;


enum Scheme {
	HTTP,
	HTTPS,
	FTP,
	OTHER
};

class Url{
	friend class HttpRequest;
public:
	Url(): m_scheme(OTHER), m_valid_url(false), 
		   m_loop_path(false) {}
	
	Url(const string &url):
		m_url(url), m_scheme(OTHER), 
		m_valid_url(false), m_loop_path(false) {
		parse_url();
	}
		
	Url(const char *url):
		m_scheme(OTHER), m_valid_url(false), 
		m_loop_path(false) {
		m_url.assign(url);
		parse_url();
	}
	
	
	~Url() {};
	
	bool valid_url();
	bool loop_path();
	bool empty() {
		return m_url.empty();
	}	
	
	void set_url(const string &url)
	{
		m_url = url;
		parse_url();
	}

	void set_url(const char *url)
	{
		m_url.assign(url);
		parse_url();
	}
	
	string get_host() {
		return m_host;
	}
	
	string get_path() {
		return m_path;
	}
	
	string get_port() {
		return m_port;
	}
	
	Scheme get_scheme() {
		return m_scheme;
	}

private:
	string          m_url;
	Scheme          m_scheme;
	string          m_host;
	string          m_path;
	string          m_port;
	
	bool            m_valid_url;	
	bool            m_loop_path;
	
	void parse_url();
	bool check_scheme();
	bool check_host();
	bool check_path();
};



#endif
