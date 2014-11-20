#include "httprequest.h"

using namespace std;

set<string>          HttpRequest::m_method_set;
map<string, string>  HttpRequest::m_default_header;

/*
 * set static member: methods and headers
 */
void HttpRequest::initial()
{
	if (m_method_set.empty()) {
		m_method_set.insert("GET");
		m_method_set.insert("POST");
		m_method_set.insert("HEAD");
		m_method_set.insert("PUT");
		m_method_set.insert("TRACE");
		m_method_set.insert("OPTIONS");
	}
	
	if (m_default_header.empty()) {
		m_default_header["Accept"] = ": text/*\r\n";
		m_default_header["Connection"] = ": close\r\n";
	}
	
	map<string, string>::iterator it;
	for (it = m_default_header.begin(); it != m_default_header.end(); ++it) {
		m_headers.insert(*it);
	}
}

/*
 * get spec header
 */
string HttpRequest::get_header(const string &name) 
{
	map<string, string>::iterator it;
	it = m_headers.find(name);
	if (it != m_headers.end())
		return it->second;
	return "";
}
	
string HttpRequest::get_header(const char *name) 
{
	string n(name);
	map<string, string>::iterator it;
	it = m_headers.find(n);
	if (it != m_headers.end())
		return it->second;
	return "";	
}

/*
 * make request content, including start line, headers
 * and content body
 */
 bool HttpRequest::make_request()
 {
 	if (m_method_set.find(m_method) == m_method_set.end())
 		return false;

	if (m_url.m_url.empty())
		return false;

	/* start line */
 	m_request.append(m_method);
 	m_request.append(1, ' ');
 	m_request.append(m_url.get_path());
 	m_request.append(1, ' ');
 	if (m_httpversion == 1)
 		m_request.append("HTTP/1.1\r\n");
 	else
 		m_request.append("HTTP/1.0\r\n");
 	
 	/* add header */
 	m_request.append("Host: ");
 	m_request.append(m_url.get_host());
 	m_request.append("\r\n");
 	map<string, string>::iterator it;
 	for (it = m_headers.begin(); it != m_headers.end(); ++it) {
 		m_request.append(it->first);
 		m_request.append(it->second);
 	}
 	/* end flag */
 	m_request.append("\r\n");  
 	
 	/* body */
 	if (!m_body.empty())
 		m_request.append(m_body);
 	
 	return true;
 }
 
 
/*
 * for debug
 */
void HttpRequest::print_all(ostream &out)
{
	out << "Method: " << m_method << endl;

	out << "Headers: " << endl;
	map<string, string>::iterator it;
	for (it = m_headers.begin(); it != m_headers.end(); ++it)
		out << it->first << it->second;
	
	make_request();
	out << "total request--------->" << endl;
	out << m_request << endl;

}
 
 
 
 
 
