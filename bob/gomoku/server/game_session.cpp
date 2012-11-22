#include <bob/gomoku/server/game_session.hpp>

#include <string>
#include <queue>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include <bob/database/database.hpp>

#include <bob/gomoku/message.hpp>
#include <bob/gomoku/game/board.hpp>
#include <bob/gomoku/game/replay.hpp>
#include <bob/gomoku/server/game_participant.hpp>

typedef unsigned int uint;

namespace bob { namespace gomoku { namespace server
{

game_session::game_session
    ( game_participant_ptr player_white
    , game_participant_ptr player_black
    , bob::database::connection& connection
    )
    : player_black_m(player_black, "<unknown>", 0u, result_draw)
    , player_white_m(player_white, "<unknown>", 0u, result_draw)
    , board_settings_m(50, 50)
    , board_m(board_settings_m)
    , connection_db_m(connection)
{
}

void game_session::start()
{
    using namespace bob::gomoku;

    std::srand(std::time(0));
    if (std::rand() % 2)
    {
        std::swap(player_white_m, player_black_m);
    }

    try
    { 
        authenticate();
        
        start_game();
        start_game();
        start_game();
    }
    catch(...)
    {
        report_draw("oponnent_disconnect");
    }
}


void game_session::start_game()
{
    std::string message = std::to_string(board_m.settings().rows()) + " " +
                          std::to_string(board_m.settings().columns()) + '\n';

    // Sending the board settings.
    player_white_m.connection->send_message(message);
    player_black_m.connection->send_message(message);

    // Sending the start signal.
    player_black_m.connection->send_message("black\n");
    player_white_m.connection->send_message("white\n");
    
    detail::game_participant_data* curr = &player_black_m;
    detail::game_participant_data* next = &player_white_m;
    while (true)
    { 
        bool error_parse = false;
        
        message = curr->connection->read_message();
        bob::gomoku::message_move move = bob::gomoku::client_message::parse_move(message, error_parse);
        
        // The message was in wrong format.
        if (error_parse)
        {
            report_over(next, curr, "invalid_input: \"" + message + "\"");
            break;
        }

        // The move was invalid.
        if (!board_m.play(move))
        {
            report_over(next, curr, "invalid_move: \"" + message + "\"");
            break;
        }

        // The move was winning.
        if (board_m.over())
        {
            report_over(curr, next, "last_move: \"" + message + "\"");
            break;
        }
        
        // Send the move to the opponent.
        next->connection->send_message(bob::gomoku::client_message::make_move(move));
        std::swap(curr, next);
    }
    
    std::swap(player_white_m, player_black_m);
    board_m.clear(board_settings_m);
}

void game_session::report_draw(std::string const& info, bool replay)
{
    player_black_m.result = result_draw;
    player_white_m.result = result_draw;

    std::string replay_info;
    if (replay)
    {
        replay_info = "; replay_id: " + std::to_string(save_game_session());
    }
    
    try
    {
        player_white_m.connection->send_message("over d; " + info + replay_info + "\n");
        player_black_m.connection->send_message("over d; " + info + replay_info + "\n");
    }
    catch(...)
    {
    }
}

void game_session::report_over
    ( detail::game_participant_data* player_w
    , detail::game_participant_data* player_l
    , std::string const& info
    , bool replay
    )
{
    player_w->result = result_win;
    player_l->result = result_loss;

    std::string replay_info;
    if (replay)
    {
        replay_info = "; replay_id: " + std::to_string(save_game_session());
    }
    
    try
    {
        player_l->connection->send_message("over l; " + info + replay_info + "\n");
        player_w->connection->send_message("over w; " + info + replay_info + "\n");
    }
    catch(...)
    {
    }
}

uint game_session::save_game_session()
{
    using bob::database::game_session_participant_data;
   
    uint replay_id = 0;
    try
    {
        game_session_participant_data ps[2] =
            { game_session_participant_data(player_black_m.id, 0, player_black_m.result)
            , game_session_participant_data(player_white_m.id, 1, player_white_m.result)
            };

        replay_id = connection_db_m.insert_game_session
                        ( bob::database::game_type::gomoku
                        , ps, ps + 2
                        , bob::gomoku::game::replay_data
                            ( board_m
                            )
                        );
    }
    catch(std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return replay_id;
}

void game_session::authenticate()
{
    // Read theauthentication data (name, password).
    player_white_m.name = player_white_m.connection->read_message();
    player_black_m.name = player_black_m.connection->read_message();
    
    // Try to authenticate (or register) the user.
    bool auth1 = true;
    bool auth2 = true;
    player_black_m.id = connection_db_m.authenticate_user(player_black_m.name, auth1);
    player_white_m.id = connection_db_m.authenticate_user(player_white_m.name, auth2);

    if (!auth1 || !auth2)
    {
        throw std::runtime_error("Could not authenticate");
    }
}

game_session_pool::game_session_pool(uint thread_count)
    : work_m(io_service_m)
{
    for (uint i = 0; i < thread_count; ++i)
    {
        threads_m.create_thread(boost::bind(&boost::asio::io_service::run, &io_service_m));
    }
}

game_session_pool::~game_session_pool()
{
    io_service_m.stop();
    threads_m.join_all();
}

void game_session_pool::add_game_session
    ( game_participant_ptr player_white
    , game_participant_ptr player_black
    , bob::database::connection& connection
    )
{
    game_session_ptr new_session(new game_session(player_white, player_black, connection));
    io_service_m.post(std::bind(&game_session::start, new_session));
}

} // namespace server
} // namespace gomoku
} // namespace bob

