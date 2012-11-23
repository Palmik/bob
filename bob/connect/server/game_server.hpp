#ifndef BOB_GOMOKU_SERVER_SERVER_HPP
#define BOB_GOMOKU_SERVER_SERVER_HPP

#include <bob/database/database.hpp>
#include "game_participant.hpp"
#include "game_session.hpp"

#include <string>
#include <queue>
#include <boost/asio.hpp>

namespace bob { namespace connect { namespace server
{

class game_server
{
public:
    game_server
        ( boost::asio::io_service& io_service
        , boost::asio::ip::tcp::endpoint const& endpoint
        , std::function<void(boost::system::error_code)> const& error_callback_m
        );

    void start();

private:
    void do_start_accept();
    void handle_accept
        ( game_participant_ptr player
        , boost::system::error_code const& e
        );

    boost::asio::io_service& io_service_m;
    boost::asio::ip::tcp::acceptor acceptor_m;
    std::function<void(boost::system::error_code)> error_callback_m;
    std::list<game_participant_ptr> game_participant_queue_m;

    game_session_pool session_pool_m;

    bob::database::connection connection_m;
};

inline void game_server::start()
{
    do_start_accept();
}

} // namespace server
} // namespace connect
} // namespace bob

#endif // BOB_GOMOKU_SERVER_SERVER_HPP

