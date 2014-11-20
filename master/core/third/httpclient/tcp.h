#ifndef _TCP_H_
#define _TCP_H_

#include <netdb.h>                    /* gethostbyname(), gethostbyaddr(), getaddrinfo() */
#include <sys/socket.h>
#include <cstring>
#include <signal.h>
#include <unistd.h>

using namespace std;

const unsigned int TIMEOUT_SECONDS = 10;
const unsigned int SLEEP_SECONDS   = 20;

const unsigned int INFATAL_ERROR   = 1;
const unsigned int FATAL_ERROR     = 2;
const unsigned int SUCCESS         = 0;

const unsigned int MAX_LINE        = 100;

typedef void Sigfunc(int);

class TcpClient {
	friend class HttpClient;
public:
	TcpClient(unsigned int time = TIMEOUT_SECONDS):
	m_sockfd(-1), m_connected(false), m_timewait(time), m_signal_func(NULL) {
		bzero(&m_hints, sizeof(struct addrinfo));
		/* ingore SIGPIPE sig */
		m_signal_func = signal(SIGPIPE, SIG_IGN);
		
		m_hints.ai_family = AF_UNSPEC;
		m_hints.ai_socktype = SOCK_STREAM;
	}
	
	~TcpClient() {
		/* recover the SIGPIPE handle function */
		if (m_signal_func != SIG_ERR) {
			signal(SIGPIPE, m_signal_func);
			m_signal_func = NULL;	
		}
	}
	
	int                 readn(char *buffer, size_t n);
	int                 writen(const char *buffer, size_t n);
	int                 read_line(char *buffer);
	unsigned int        connect_server(const char *host, const char *serv);
	unsigned int        check_errno();
	
	bool close_sock() {
		if (close(m_sockfd) == -1)
			return false;
		m_sockfd = -1;
		return true;
	}
	
private:
	int                 m_sockfd;
	
	struct addrinfo     m_hints;
	
	bool                m_connected;
	
	unsigned int        m_timewait;
	Sigfunc             *m_signal_func;
	
	unsigned int        do_connect(const struct sockaddr *servaddr, socklen_t addrlen);
	void                set_timeout();
	
	void  set_timeout_sec(unsigned int seconds) {
		m_timewait = seconds;
	}
};

#endif /* _TCP_H_ */
