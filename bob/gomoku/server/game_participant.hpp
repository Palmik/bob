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
    game_participant(boost::shared_ptr<boost::asio::ip::tcp::socket> socket);
    ~game_participant();

    bool is_open() const;

    std::string read_message();
    void        send_message(std::string const& m);

private:
    boost::shared_ptr<boost::asio::ip::tcp::socket> socket_m;

    boost::asio::streambuf message_buffer_m;

    static const char delimiter_m = '\n';
};

typedef std::shared_ptr<game_participant> game_participant_ptr;

inline game_participant::game_participant(boost::shared_ptr<boost::asio::ip::tcp::socket> socket)
    : socket_m(socket)
{
}

inline game_participant::~game_participant()
{
    socket_m->close();
}

inline bool game_participant::is_open() const
{
    return socket_m->is_open();
}

inline std::string game_participant::read_message()
{
    try
    {
        boost::asio::read_until(*socket_m, message_buffer_m, delimiter_m);
    }
    catch(...)
    {
        socket_m->close();
        throw;
    }

    std::istream is(&message_buffer_m);
    std::string message_raw;
    std::getline(is, message_raw, delimiter_m);

    return message_raw;
}

inline void game_participant::send_message(std::string const& m)
{
    try
    {
        boost::asio::write
            ( *socket_m
            , boost::asio::buffer(m.data(), m.size())
            );
    }
    catch(...)
    {
        socket_m->close();
        throw;
    }
}

} // namespace server
} // namespace gomoku
} // namespace bob

#endif // BOB_GOMOKU_SERVER_GAME_PARTICIPANT_HPP
