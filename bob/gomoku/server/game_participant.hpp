#ifndef BOB_GOMOKU_SERVER_GAME_PARTICIPANT_HPP
#define BOB_GOMOKU_SERVER_GAME_PARTICIPANT_HPP

#include <string>
#include <queue>
#include <memory>
#include <boost/asio.hpp>

namespace bob { namespace gomoku { namespace server
{

class game_participant
{
public:
    game_participant
        ( boost::asio::io_service& io_service
        , std::function<void(boost::system::error_code)> const& error_callback
        );

    ~game_participant();

    boost::asio::ip::tcp::socket& socket();

    std::string const& name() const;

    std::string read_message(bool& error);
    void        send_message(std::string const& m, bool& error);

private:
    boost::asio::io_service& io_service_m;
    boost::asio::ip::tcp::socket socket_m;

    boost::asio::streambuf message_buffer_m;

    static const char delimiter_m = '\n';
};

typedef std::shared_ptr<game_participant> game_participant_ptr;

inline game_participant::game_participant
    ( boost::asio::io_service& io_service
    , std::function<void(boost::system::error_code)> const& error_callback
    )
    : io_service_m(io_service)
    , socket_m(io_service)
{
}

inline game_participant::~game_participant()
{
    socket_m.close();
}

inline boost::asio::ip::tcp::socket& game_participant::socket()
{
    return socket_m;
}

inline std::string game_participant::read_message(bool& error)
{
    error = false;
    try
    {
        boost::asio::read_until(socket_m, message_buffer_m, delimiter_m);
    }
    catch(...)
    {
        error = true;
        socket_m.close();
    }

    std::istream is(&message_buffer_m);
    std::string message_raw;
    std::getline(is, message_raw, delimiter_m);

    return message_raw;
}

inline void game_participant::send_message(std::string const& m, bool& error)
{
    error = false;
    try
    {
        boost::asio::write
            ( socket_m
            , boost::asio::buffer(m.data(), m.size())
            );
    }
    catch(...)
    {
        error = true;
        socket_m.close();
    }
}

} // namespace server
} // namespace gomoku
} // namespace bob

#endif // BOB_GOMOKU_SERVER_GAME_PARTICIPANT_HPP
