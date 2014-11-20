#include "url.h"
#include <iostream>
#include <set>

using namespace std;

bool Url::valid_url()
{
	if (m_valid_url)
		return true;

	if (!check_scheme() || !check_host() || !check_path())
		return false;
	
	m_valid_url = true;
	return true;
}

/*
 * parse the given url, and set members
 */
void Url::parse_url()
{
 	/* get the scheme */
 	m_port = "80";
	string::size_type scheme_end_index = m_url.find("://");
	if (scheme_end_index == string::npos) {
		m_scheme = HTTP;
	}
	else {
		string scheme = m_url.substr(0, scheme_end_index);
		if (scheme == "http") {
			m_scheme = HTTP;
		}
		else if (scheme == "https") {
			m_scheme = HTTPS;
			m_port = "443";
		}
		else if (scheme == "ftp") {
			m_scheme = FTP;
		}
		else 
			m_scheme = OTHER;
	}
	
	string no_scheme_url;
	/* get the host */
	if (scheme_end_index == string::npos)
		no_scheme_url = m_url;
	else 
		no_scheme_url = m_url.substr(scheme_end_index+3);
		
	/* store the host in m_host, just host, no uri path */	
	string::size_type host_end_index = no_scheme_url.find("/");
	if (host_end_index == string::npos)
		m_host = no_scheme_url;
	else 
		m_host = no_scheme_url.substr(0, host_end_index);
	
	/* get the path */
	if (host_end_index == string::npos)
		m_path = "/";
	else 
		m_path = no_scheme_url.substr(host_end_index);
	
	/* get port if url has spec port number */
	string::size_type port_index = m_host.find(":");
	if (port_index != string::npos)
		m_port = m_host.substr(port_index+1);	
		
	/* print for debug *
	cout << "URL:    " << m_url << endl;
	
	if (m_scheme == HTTP)
		cout << "scheme: " << "http" << endl;
	else if (m_scheme == FTP)
		cout << "scheme: " << "ftp" << endl;
	else 
		cout << "scheme: " << "other" << endl;
		
	cout << "host:   " << m_host << endl;
	cout << "port:   " << m_port << endl;
	cout << "path:   " << m_path << endl;
	/* end */

}


bool Url::check_scheme()
{
	//...
	return true;
}

bool Url::check_host()
{
	//...
	return true;
}

bool Url::check_path()
{
	//...
	return true;
}

/*
 * this function check the given path to see wether it
 * is a loop, this is useful for a crwaler program
 */
bool Url::loop_path()
{
	if (m_loop_path)
		return true;
		
	string::size_type start_index = 0;
	set<string> suburl;
	
	string::size_type length = m_path.size();
	
	if (length < 20)
		return false;
	
	string::size_type i = 0;
	bool in_path = false;
	unsigned int counter = 0;
	for ( ; i < length; ++i) {
		if (m_path[i] == '/' && !in_path) {
			start_index = i;
			in_path = true;
		}
		else if (m_path[i] == '/' && in_path) {
			suburl.insert(m_path.substr(start_index+1, i - start_index - 1));
			start_index = i;
			++counter;
		}
	}
	if (suburl.size() == 0 && counter == 0)
		return false;
	
	if (suburl.size() <= counter / 2) {
		m_loop_path = true;
		return true;	
	}
	return false;
}






