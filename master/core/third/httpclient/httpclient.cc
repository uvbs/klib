#include "httpclient.h"
#include <zlib.h>     /* inflate, inflateInit2, inflateEnd */

using namespace std;

/*
 * constructor
 */
HttpClient::HttpClient()
{
	m_buffer = new char[BUF_SIZE];
	
	clear();
}

/*
 * clear members, prepare for next http transction
 */
void HttpClient::clear()
{
	m_error          = 0;
	m_error_type     = 0;
	m_content_length = 0;
	m_relocated      = false;
	m_content        = false;
	m_chunked        = false;
	m_compressed     = false;
	m_reloc_times    = 0;
}

/*
 * execute a http transction
 */
bool HttpClient::execute(HttpRequest &request, HttpResponse &response)
{
	if (!request.make_request()) {
		m_error = REQUEST_ERROR;
		return false;
	}

	if (!send_request(request))
		return false;
			
	if (!recv_response(response))
		return false;
		
	/* relocated case */
	if (m_relocated) {
		if (!handle_reloc(request, response)) 
			return false;
	}
		
	return true;
}

bool HttpClient::send_request(HttpRequest &request)
{
	string host = request.m_url.get_host();
	string port = request.m_url.get_port();
	
	unsigned int retval = m_tcp.connect_server(host.c_str(), port.c_str());
	if (retval != SUCCESS) {
		m_error = CONN_ERROR;
		m_error_type = retval;		
		return false;
	}
	
	if (m_tcp.writen(request.m_request.c_str(), request.m_request.size()) == -1) {
		m_error = SEND_ERROR;
		m_error_type = m_tcp.check_errno();	
		return false;
	}
	
	return true;
}

bool HttpClient::recv_response(HttpResponse &response)
{
	memset(m_buffer, 0, BUF_SIZE);
	
	if (!read_header(response)) {
		m_error = READ_ERROR;
		m_error_type = m_tcp.check_errno();
		return false;
	}

	if (parse_response(response)) {
		if (!read_content(response))
			return false;	
	}
	return true;	
}

/*
 *	read start line and headers, store in response
 */
bool HttpClient::read_header(HttpResponse &response)
{
	char *current = m_buffer;
	int read_bytes = 0;
	int total_bytes = 0;
	
	read_bytes = m_tcp.read_line(current);
	if (read_bytes < 0)
		return false;
	response.set_status_line(current);
	current += read_bytes;
	total_bytes += read_bytes;
	

	
	while (1) {
		read_bytes = m_tcp.read_line(current);
		if (read_bytes < 0)  /* error */
			return false;
		if (read_bytes > 0) {
			/* add header to response object, this maybe just "\r\n"*/
			if (!response.set_header(current))
				break;
			current += read_bytes;
			total_bytes += read_bytes;
		}
		if (read_bytes == 0) /* may never happen */
			break;
	}
	
	/* clear buffer for reading content body */
	memset(m_buffer, 0, total_bytes);
	return true;
}

/*
 * resolve the status line and headers, to see wether the source
 * is relocted or not. do we need read a content body?  is the
 * content compressed? is it chunked?
 */
bool HttpClient::parse_response(HttpResponse &response)
{
	/* good response */
	unsigned int status = response.m_status;
	if (status >= 200 && status < 300) {
		/* 200, 203, 206 means response contains content body */
		if (status == 200 || status == 203 || status == 206)
			m_content = true;
	}
	else if (status >= 300 && status < 400) {
		m_relocated = true;
	}
	else if (status >= 400 && status < 500) {
	
	}
	else if (status >= 500 && status < 600) {
	
	}
	
	if (response.m_headers.find("Content-Length") != response.m_headers.end()) {
		string length = response.m_headers["Content-Length"];
		for (unsigned int i = 0; i < length.size(); ++i)
			m_content_length = m_content_length*10 + length[i] - '0';
		
		if (m_content_length > BUF_SIZE) {
			m_error = BUFF_ERROR;
			return false;
		}
	}
	
	if (response.m_headers.find("Transfer-Encoding") != response.m_headers.end())
		m_chunked = true;
	if (response.m_headers.find("Content-Encoding") != response.m_headers.end())
		m_compressed = true;
		
	return m_content;
}

/*
 * read content body, handle compressed, chunked case
 */
bool HttpClient::read_content(HttpResponse &response)
{
	unsigned int read_bytes = 0;
	if (m_chunked) {
		if (!handle_chunk()) {
			m_error = CHUNK_ERROR;
			return false;
		}
	}
	else {
		read_bytes = m_tcp.readn(m_buffer, m_content_length);
		if (read_bytes < 0) {
			m_error = READ_ERROR;
			m_error_type = m_tcp.check_errno();
			return false;
		}
		if (read_bytes < m_content_length)
			;//
		m_content_begin = m_buffer;
	}
	
	if (m_compressed)
		if (!decompress()) {
			m_error = DECOM_ERROR;
			return false;
		}
	
	response.set_body(m_content_begin, m_content_length);
	return true;	 	
}

/*
 * handle chunk case
 */
bool HttpClient::handle_chunk()
{
	char *current  = m_buffer;
	int read_bytes = 0;
	int total      = 0;
	int chunk_size = 0;
	char c;
	int rc;
	
	while (1) {
		/* read the chunk size */
		rc = read(m_tcp.m_sockfd, &c, 1);
		if (rc < 0)
			return false;
		if (rc == 0) /* never happen */
			break;
		if (c >= '0' && c <= '9') {
			chunk_size = chunk_size * 16 + (c - '0');
			continue;
		}
		if (c >= 'a' && c <= 'z') {
			chunk_size = chunk_size * 16 + (c - 'a');
			continue;
		} 
		if (c >= 'A' && c <= 'Z') {
			chunk_size = chunk_size * 16 + (c - 'A');
			continue;
		}
		/* in the end of a chunk, we  read the real content if it's */
		/* not the last zero chunk                                  */
		if (c == '\n') {
			if (chunk_size == 0)
				break;
			else {
				read_bytes = m_tcp.readn(current, chunk_size);
				if (read_bytes < 0)
					return false;
				if (read_bytes < chunk_size) { /* may never happen */
					;
//					out_stream << "read " << read_bytes << " less than the chunk "
//					           << chunk_size << endl;
				}
				total += read_bytes;
				current += read_bytes;
				read_bytes = 0;
				
				/* skip '\r\n' in the end */
				read(m_tcp.m_sockfd, &c, 1);
				read(m_tcp.m_sockfd, &c, 1);
			}
		}		
	}
	
	m_content_begin = m_buffer;
	m_content_length = total;	
	return true; 
}

/*
 * Description : decompress the gzip data
 */
bool GzipDecompress(Byte *source, uLong source_length, Byte *dest, uLong *dest_length)
{
	int err = 0;
	z_stream d_stream = {0}; /* decompression stream */
	static char dummy_head[2] =
	{
	0x8 + 0x7 * 0x10,
	(((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
	};
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;
	d_stream.next_in = source;
	d_stream.avail_in = 0;
	d_stream.next_out = dest;
	if(inflateInit2(&d_stream, 47) != Z_OK) 
		return false;
	while (d_stream.total_out < *dest_length && d_stream.total_in < source_length) {
		d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
		if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) 
			break;
		if(err != Z_OK )
		{
			if(err == Z_DATA_ERROR)
			{
				d_stream.next_in = (Bytef*) dummy_head;
				d_stream.avail_in = sizeof(dummy_head);
				if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK)
				{
					return false;
				}
			}
			else 
				return false;
		}
	}
	if(inflateEnd(&d_stream) != Z_OK) 
		return false;
	*dest_length = d_stream.total_out;
	return true;
}

/*
 * decompress the data
 */
bool HttpClient::decompress()
{
	char *tmp_buffer = new char[BUF_SIZE];
	memset(tmp_buffer, 0, BUF_SIZE);
	
	unsigned long data_length = 0;
	if (!GzipDecompress((Byte*)m_buffer, m_content_length, (Byte*)tmp_buffer, &data_length))
		return false;
	
	m_content_begin = tmp_buffer;
	m_content_length = data_length;
	return true;
}

/*
 * hanle relocation
 */
bool HttpClient::handle_reloc(HttpRequest &request, HttpResponse &response)
{
	if (m_reloc_times >= MAX_RELOC_TIMES) {
		m_error = RELOC_ERROR;
		return false;
	}
	string url = response.m_headers["Location"];
	Url reloc_url(url);
	
	request.reset(reloc_url);
	response.clear();
	m_reloc_times++;
	/* the reloc url and previous url share a same host, then we need not shutdown */
	/* the connection  */
	if (reloc_url.get_host().empty() || reloc_url.get_host() == request.m_url.get_host()) {
		return execute(request, response);
	}
	else {
		m_tcp.close_sock();
		return execute(request, response);
	}
}

/*
 * if execute failed, call this func to find reason
 */
void HttpClient::check_error(ostream &out)
{
	switch (m_error) {
		case REQUEST_ERROR:
			out << "bad http request" << endl;
			break;
		case CONN_ERROR:
			out << "connect error, ";
			if (m_error_type == 1)
				out << "try it later";
			out << endl;
			break;
		case SEND_ERROR:
			out << "send request failed, ";
			if (m_error_type == 1)
				out << "try it later";
			out << endl;
			break;
		case READ_ERROR:
			out << "read response failed, ";
			if (m_error_type == 1)
				out << "try it later";
			out << endl;
			break;
		case DECOM_ERROR:
			out << "decompressed data failed" << endl;
			break;
		case CHUNK_ERROR:
			out << "dechunk failed" << endl;
			break;
		case RELOC_ERROR:
			out << "handle relocation failed" << endl;
			break;
		case BUFF_ERROR:
			out << "buffsize is too small" << endl;
			break;
		default:
			break;
	}	
}





