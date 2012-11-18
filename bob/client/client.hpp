#ifndef BOB_CLIENT_CLIENT_HPP
#define BOB_CLIENT_CLIENT_HPP

#include <string>
#include <functional>
#include <queue>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>


namespace bob
{

class client
{
public:
    client( boost::asio::io_service& io_service
          , std::function<void(std::string)> const& read_callback
          , std::function<void(boost::system::error_code)> const& error_callback
          );

    ~client();

    void connect(boost::asio::ip::tcp::resolver::iterator endpoint_iterator);    
    void disconnect();
    void send(std::string const& m);

private:
    void do_connect(boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
    void do_send(std::string const&); 
    void do_read();
    void handle_connect(boost::system::error_code const& e);
    void handle_send(boost::system::error_code const& e);
    void handle_read(boost::system::error_code const& e);

    boost::asio::io_service& io_service_m;
    boost::asio::ip::tcp::socket socket_m;
    std::function<void(std::string)> read_callback_m;
    std::function<void(boost::system::error_code)> error_callback_m;

    std::queue<std::string> message_queue_m;
    boost::asio::streambuf  message_buffer_m;

    static const char delimiter_m = '\n';
};


inline client::client
    ( boost::asio::io_service& io_service
    , std::function<void(std::string)> const& read_callback
    , std::function<void(boost::system::error_code)> const& error_callback
    )
    : io_service_m(io_service)
    , socket_m(io_service)
    , read_callback_m(read_callback)
    , error_callback_m(error_callback)
{
}

inline client::~client()
{
    disconnect();
}

inline void client::connect(boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
{
    do_connect(endpoint_iterator);
}

inline void client::disconnect()
{
    socket_m.close();
}

inline void client::send(std::string const& m)
{
    io_service_m.post(std::bind(&client::do_send, this, m));
}

} // namespace bob

#endif // BOB_CLIENT_CLIENT_HPP

