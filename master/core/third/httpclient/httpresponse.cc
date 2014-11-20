#include "httpresponse.h"
#include <iostream>

using namespace std;

/*
 * clear members
 */
void HttpResponse::clear()
{
	m_status_line = "";
	m_reason      = "";
	m_httpversion = 1;
	m_status      = 0;
	m_body        = NULL;
	m_body_size   = 0;
}

/*
 * resolve first line of http response, get info, store in httpreaponse
 * status line is like 'HTTP/1.x xxx reason phrase\r\n'
 */
void HttpResponse::set_status_line(char *buf)
{
	char *cursor = buf;
	
	/* get version info */
	cursor += 7;
	m_httpversion = *cursor - '0';
	
	/* get status number */
	cursor += 2;
	while (*cursor >= '0' && *cursor <= '9') {
		m_status = m_status * 10 + *cursor - '0';
		cursor++;
	}
	
	/* get reason phrase */
	cursor++;
	unsigned int size = 0;
	while (*(cursor+size) != '\r' && *(cursor+size) != '\n')
		size++;
	m_reason.assign(cursor, size);
	
	/* get whole line */
	cursor += size;
	m_status_line.assign(buf, cursor-buf);
}

/*
 * resolve response header, one header each time
 * header is like this, 'Content-Length: 9527\r\n'
 */
bool HttpResponse::set_header(char *buf)
{
	/* end line '\r\n' */
	if (*buf == '\r' && *(buf+1) == '\n')
		return false;
		
	char *cursor = buf;
	unsigned int name_size = 0;
	while (buf[name_size] != ':') {
		cursor++;
		name_size++;
	}
	string name(buf, name_size);
	
	while (!cursor == ' ')
		cursor++;
	unsigned int value_size = 0;
	while (cursor[value_size] != '\r' && cursor[value_size] != '\n')
		value_size++;
	string value(cursor, value_size);
	
	m_headers[name] = value;
	return true;
}

/*
 * debug
 */
void HttpResponse::print_all(ostream &out)
{
	out << "status line: " << m_status_line << endl;
	out << "http version: HTTP/1." << m_httpversion << endl;
	out << "status code: " << m_status << endl;
	out << "reason phrase: " << m_reason << endl;

	out << "--------------------sep--------------------" << endl;
	out << "below is response headers:" << endl;
	map<string, string>::iterator it;
	for (it = m_headers.begin(); it != m_headers.end(); ++it)
		out << it->first << ": " << it->second << endl;
		
	out << "--------------------sep--------------------" << endl;
	out << "below is content:" << endl;
	out << m_body << endl;
}



