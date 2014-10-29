#include "stdafx.h"
#include "https_downloader.h"

#include "../../common/public/url.h"


#include <boost/asio.hpp>
using namespace boost::asio;

#include <boost/asio/ssl.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>


enum { max_length = 1024 };

#ifdef _WIN32
#pragma comment(lib, "ssleay32.lib")
#pragma comment(lib, "libeay32.lib")
#else
#endif


class down_client : public boost::enable_shared_from_this<down_client>
{
public:
    down_client(url_parser& parser,
        const std::string& path,
        boost::asio::io_service& io_service,
        boost::asio::ssl::context& context,
        boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
        : m_url_parser(parser)
        , m_socket(io_service, context)
        , m_down_succ(false)
        , m_find_data(false)
        , m_hfile(nullptr)
        , m_failed_reason(e_failed_unknow)
        , m_content_length(0)
        , m_writed_length(0)
        , m_check_alive_thread(boost::bind(&down_client::check_alive_worker, this))
    {
        m_down_path = path;
        m_last_recv_time = time(nullptr);
        m_bstop_check_alive = false;

        // 打开文件
        if (nullptr == m_hfile)
        {
            m_hfile = fopen(m_down_path.c_str(), "wb");
            if (nullptr == m_hfile)
            {
                m_failed_reason = e_failed_create_file;
                return;
            }
        }
        
        m_socket.set_verify_mode(boost::asio::ssl::verify_peer);
        m_socket.set_verify_callback(
            boost::bind(&down_client::verify_certificate, this, _1, _2));

        boost::asio::async_connect(m_socket.lowest_layer(), endpoint_iterator,
            boost::bind(&down_client::handle_connect, this,
            boost::asio::placeholders::error));
    }
    ~down_client()
    {
        if (nullptr != m_hfile)
        {
            fclose(m_hfile);
            m_hfile = nullptr;
        }
    }

    bool verify_certificate(bool preverified,
        boost::asio::ssl::verify_context& ctx)
    {
        // The verify callback can be used to check whether the certificate that is
        // being presented is valid for the peer. For example, RFC 2818 describes
        // the steps involved in doing this for HTTPS. Consult the OpenSSL
        // documentation for more details. Note that the callback is called once
        // for each certificate in the certificate chain, starting from the root
        // certificate authority.

        // In this example we will simply print the certificate's subject name.
        // char subject_name[256];
        // X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
        // X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
        // std::cout << "Verifying " << subject_name << "\n";

        // 不进行证书的校验
        return true;
    }

    void handle_connect(const boost::system::error_code& error)
    {
        if (!error)
        {
            m_socket.async_handshake(boost::asio::ssl::stream_base::client,
                boost::bind(&down_client::handle_handshake, this,
                boost::asio::placeholders::error));
        }
        else
        {
            m_failed_reason = e_failed_connect;
        }
    }

    void handle_handshake(const boost::system::error_code& error)
    {
        if (!error)
        {
            std::ostringstream str_stream;

            str_stream << "GET ";
            str_stream << m_url_parser.get_path();
            std::string str_query = m_url_parser.get_query();
            if (!str_query.empty())
            {
                str_stream << "?" << str_query;
            }
            str_stream << " HTTP/1.1\r\n";

            str_stream << "Host: ";
            str_stream << m_url_parser.get_host();
            if (m_url_parser.get_port() != "443")
            {
                str_stream << ":" << m_url_parser.get_port();
            }
            str_stream << "\r\n";

            str_stream << "Accept: */*\r\n";
            str_stream << "Connection: close\r\n";            
            str_stream << "\r\n";

            std::string str_request = str_stream.str();

            boost::asio::async_write(m_socket,
                boost::asio::buffer(str_request, str_request.size()),
                boost::bind(&down_client::handle_write, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));

            m_last_recv_time = time(nullptr);
        }
        else
        {
            m_failed_reason = e_failed_handshake;
        }
    }

    void handle_write(const boost::system::error_code& error,
        size_t bytes_transferred)
    {
        if (!error)
        {
            m_socket.async_read_some(
                boost::asio::buffer(m_reply, sizeof(m_reply)),
                boost::bind(&down_client::handle_read, 
                            shared_from_this(), 
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));

            m_last_recv_time = time(nullptr);
        }
        else
        {
            m_failed_reason = e_failed_write;
            m_socket.next_layer().close();
        }
    }

    void handle_read(const boost::system::error_code& error,
        size_t bytes_transferred)
    {
        if (!error)
        {
            if (!m_find_data)
            {
                m_response.append(m_reply, bytes_transferred);
                size_t pos = m_response.find("\r\n\r\n") ;
                if (pos != std::string::npos)
                {
                    m_find_data = true;

                    // 验证返回值
                    //HTTP/1.1 302 
                    std::string str_ret_code = Regex::get_mid(m_response, "HTTP/1.1 ", " ");
                    if (str_ret_code.empty())
                    {
                        str_ret_code = Regex::get_mid(m_response, "HTTP/1.0 ", " ");
                    }
                    size_t code = osaa::strtoui64(str_ret_code.c_str(), nullptr, 10);
                    if (200 != code)
                    {
                        return;
                    }

                    // 验证长度
                    std::string str_length = Regex::get_mid(m_response, "Content-Length:", "\r\n");
                    m_content_length = osaa::strtoui64(str_length.c_str(), nullptr, 10);
                    if (0 == m_content_length)
                    {
                        return;
                    }

                    pos += 4;
                    size_t bytes_2_write = m_response.size() - pos;
                    const char* write_pos = m_response.c_str() + pos;
                    fwrite(write_pos, 1, bytes_2_write, m_hfile);
                    m_writed_length.set(bytes_2_write);
                }
                
                if (m_response.size() >= 8000)
                {
                    m_failed_reason = e_failed_response;
                    return;
                }
            }
            else
            {
                fwrite(m_reply, 1, bytes_transferred, m_hfile);
                m_writed_length.add(bytes_transferred);

                m_down_succ = (m_content_length > 0 && m_writed_length.get() >= m_content_length);
                if (m_down_succ)
                {
                    m_socket.next_layer().close();
                }
            }
            
            m_socket.async_read_some(
                boost::asio::buffer(m_reply, sizeof(m_reply)),
                boost::bind(&down_client::handle_read, 
                            shared_from_this(), 
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));

            m_last_recv_time = time(nullptr);
        }
        else
        {
            CSASSERT(2 == error.value());
            if (2 != error.value())
            {
                m_failed_reason = e_failed_unknow;
            }
            m_socket.next_layer().close();
        }
    }

    bool get_down_succ() const { return m_down_succ; }
    e_failed_reason get_failed_reason() const  { return m_failed_reason; }

    void stop_check_alive()
    {
        m_bstop_check_alive = true;
        m_check_alive_thread.Join();
    }
    
protected:
    void check_alive_worker()
    {
        while (!m_bstop_check_alive)
        {
            Sleep(500);

            // 超时后关闭，并退出线程
            time_t now = time(nullptr);
            if (now - m_last_recv_time >= 2*60)
            {
                CSLOG(CSHPL_LOG_WARNING, "antvir socket timeout --> close socket !!!");
                m_socket.next_layer().close();
                break;
            }
        }
    }

private:
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket>  m_socket;
    url_parser&                                             m_url_parser;
    char m_request[max_length];
    char m_reply[4*max_length];

    std::string                 m_down_path;            // 下载的路径
    bool                        m_down_succ;            // 是否下载成功
    bool                        m_find_data;
    FILE*                       m_hfile;
    std::string                 m_response;

    e_failed_reason             m_failed_reason;
    size_t                      m_content_length;
    osaa::atomic<size_t>        m_writed_length;

protected:
    size_t                      m_last_recv_time;       // 上次接收数据时间
    bool                        m_bstop_check_alive;    // 是否停止检测是否是活的
    osaa::CThread               m_check_alive_thread;
};


https_downloader::https_downloader()
{
}

https_downloader::~https_downloader(void)
{
}

bool https_downloader::download(const std::string& url, const std::string& path)
{
    url_parser parser(url.c_str());
    std::string str_host = parser.get_host();
    std::string str_port  = parser.get_port();

    boost::asio::io_service io_service;
    bool down_result = false;

    CSLOG(CSHPL_LOG_WARNING, "antvir begin execute download!!!");

    try 
    {
        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::query query(str_host, str_port);
        boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
    
        boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
        ctx.set_default_verify_paths();

        boost::shared_ptr<down_client> c(new down_client(parser, path, io_service, ctx, iterator));

        io_service.run();

        c->stop_check_alive();
        down_result = c->get_down_succ();

        if (!down_result)
        {
            CSLOG(CSHPL_LOG_ERROR, "antvir failed on download: %s, reason: %d", 
                url.c_str(), 
                c->get_failed_reason());
        }
        else
        {
            CSLOG(CSHPL_LOG_ERROR, "antvir success on download: %s", 
                url.c_str());
        }
    }
    catch (...)
    {
        CSLOG(CSHPL_LOG_ERROR, "antvir exception on download: %s", url.c_str());
        return false;
    }

    return down_result;
}
