#include "client.hpp"

#include <boost/exception/enable_current_exception.hpp>

namespace bob
{

void client::do_connect(boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
{
    boost::asio::async_connect
        ( socket_m, endpoint_iterator
        , std::bind(&client::handle_connect, this, std::placeholders::_1)
        );
}

void client::handle_connect(boost::system::error_code const& e)
{
    if (!e)
    {
        do_read();
    }
    else
    {
        error_callback_m(e);
    }
}

void client::do_send(std::string const& m)
{
    bool sending = !message_queue_m.empty();
    message_queue_m.push(m);

    if (!sending)
    {
        handle_send(boost::system::error_code());
    }
}

void client::handle_send(boost::system::error_code const& e)
{
    if (!e)
    {
        if (!message_queue_m.empty())
        {
            std::string const& message = message_queue_m.front();
            message_queue_m.pop();

            boost::asio::async_write
                ( socket_m
                , boost::asio::buffer(message.data(), message.size())
                , std::bind(&client::handle_send, this, std::placeholders::_1)
                );
        }
    }
    else
    {
        error_callback_m(e);
    }
}

void client::do_read()
{
    boost::asio::async_read_until
        ( socket_m, message_buffer_m, delimiter_m
        , std::bind(&client::handle_read, this, std::placeholders::_1)
        );
}

void client::handle_read(boost::system::error_code const& e)
{
    if (!e)
    {
        std::istream is(&message_buffer_m);
        std::string message;
        std::getline(is, message, delimiter_m);
        read_callback_m(message);

        do_read();
    }
    else
    {
        error_callback_m(e);
    }
}

} // namespace bob
