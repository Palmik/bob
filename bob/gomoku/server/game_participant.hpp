#ifndef BOB_GOMOKU_SERVER_GAME_PARTICIPANT_HPP
#define BOB_GOMOKU_SERVER_GAME_PARTICIPANT_HPP

#include "game_message.hpp"

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

    boost::asio::ip::tcp::socket& socket();

    game_message read_message(bool& error);
    void         send_message(game_message const& m);

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

inline boost::asio::ip::tcp::socket& game_participant::socket()
{
    return socket_m;
}

inline game_message game_participant::read_message(bool& error)
{
    boost::asio::read_until(socket_m, message_buffer_m, delimiter_m);

    std::istream is(&message_buffer_m);
    std::string message_raw;
    std::getline(is, message_raw, delimiter_m);

    return parse_message(message_raw, error);
}

inline void game_participant::send_message(game_message const& m)
{
    std::ostringstream out;
    out << m.x << ' ' << m.y << '\n'; 

    boost::asio::write(socket_m, boost::asio::buffer(out.str().data(), out.str().size()));
}

} // namespace server
} // namespace gomoku
} // namespace bob

#endif // BOB_GOMOKU_SERVER_GAME_PARTICIPANT_HPP
