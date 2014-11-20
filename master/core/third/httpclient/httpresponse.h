#ifndef _HTTP_RESPONSE_
#define _HTTP_RESPONSE_

#include <string>
#include <map>
using namespace std;

class HttpResponse {
	friend class HttpClient;
public:
	HttpResponse(): m_status(0), m_httpversion(1),
		m_body(NULL), m_body_size(0) {}
	
	string get_reason() {
		return m_reason;
	}
	string get_status_line() {
		return m_status_line;
	}
	unsigned int get_version() {
		return m_httpversion;
	}
	unsigned int get_status() {
		return m_status;
	}
	
	string get_header(const string &name) {
		return m_headers[name];
	}
	string get_header(const char *name) {
		string n(name);
		return m_headers[n];
	}	
	map<string, string> all_headers() {
		return m_headers;
	}
	
	/* content body could be very large, so we return     */
	/* a pointer to avoid large data copying, the pointer */
	/* actually point to the buffer in HttpClient object  */
	char* get_body() {
		return m_body;
	}
	
	void clear();
	void print_all(ostream &out);

private:
	string               m_status_line;
	string               m_reason;
	unsigned int         m_status;	
	unsigned int         m_httpversion;
	map<string, string>  m_headers;
	char                 *m_body;
	unsigned int         m_body_size;
	
	void set_status_line(char *buf);
	bool set_header(char *buf);
	
	void set_body(char *buf, unsigned size) {
		m_body = buf;
		m_body_size = 0;
	}
};

#endif /* _HTTP_RESPONSE_ */
