#ifndef _HTTP_CLIENT_
#define _HTTP_CLIENT_

#include "httprequest.h"
#include "httpresponse.h"
#include "tcp.h"
#include <cstring>    /* memset */
#include <iostream>

using namespace std;

const unsigned int BUF_SIZE        = 1024 * 500;
const unsigned int MAX_RELOC_TIMES = 3;

/* error type */
const unsigned int REQUEST_ERROR = 1;
const unsigned int CONN_ERROR    = 2;
const unsigned int SEND_ERROR    = 3;
const unsigned int READ_ERROR    = 4;
const unsigned int DECOM_ERROR   = 5;
const unsigned int CHUNK_ERROR   = 6;
const unsigned int RELOC_ERROR   = 7;
const unsigned int BUFF_ERROR    = 8;

class HttpClient {
public:
	HttpClient();
	
	bool execute(HttpRequest &request, HttpResponse &response);

	void check_error(ostream &out);
	void clear();	
	
	void set_timeout_sec(unsigned int timeout) {
		m_tcp.set_timeout_sec(timeout);
	}
private:
	TcpClient      m_tcp;
	HttpResponse   m_response;
	unsigned int   m_error;
	unsigned int   m_error_type;
	unsigned int   m_content_length;
	char          *m_buffer;
	char          *m_content_begin;
	
	unsigned int   m_reloc_times;
	
	/* some status flag */
	bool           m_content;
	bool           m_relocated;
	bool           m_chunked;
	bool           m_compressed;

	/* forbid copy and assignment */
	HttpClient(const HttpClient&);
	HttpClient& operator= (const HttpClient&);	

	bool send_request(HttpRequest &request);
	bool recv_response(HttpResponse &response);
	bool read_header(HttpResponse &response);
	bool parse_response(HttpResponse &response);
	bool read_content(HttpResponse &response);
	bool handle_reloc(HttpRequest &request, HttpResponse &response);
	bool handle_chunk();
	bool decompress();
	
};

#endif /* _HTTP_CLIENT_ */
