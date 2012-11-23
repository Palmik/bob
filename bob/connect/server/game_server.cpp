#include "game_server.hpp"

namespace bob { namespace connect { namespace server
{

game_server::game_server
    ( boost::asio::io_service& io_service
    , boost::asio::ip::tcp::endpoint const& endpoint
    , std::function<void(boost::system::error_code)> const& error_callback
    )
    : io_service_m(io_service)
    , acceptor_m(io_service, endpoint)
    , error_callback_m(error_callback)
    , session_pool_m(4)
    , connection_m("database.sqlite")
{
}

void game_server::do_start_accept()
{
    boost::shared_ptr<boost::asio::ip::tcp::socket> new_socket(new boost::asio::ip::tcp::socket(io_service_m));
    game_participant_ptr new_player(new game_participant(new_socket));
    acceptor_m.async_accept
        ( *new_socket
        , std::bind(&game_server::handle_accept, this, new_player, std::placeholders::_1)
        );
}

void game_server::handle_accept
    ( game_participant_ptr player
    , boost::system::error_code const& e
    )
{
    if (!e)
    {
        game_participant_queue_m.push_back(player);
        if (game_participant_queue_m.size() >= 2)
        {
            game_participant_ptr player1 = game_participant_queue_m.front();
            game_participant_queue_m.pop_front();
            game_participant_ptr player2 = game_participant_queue_m.front();
            game_participant_queue_m.pop_front();

            session_pool_m.add_game_session(player1, player2, connection_m);
        }

        do_start_accept();
    }
    else
    {
        error_callback_m(e);
    }
}

} // namespace server
} // namespace connect
} // namespace bob
