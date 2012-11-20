#ifndef BOB_GOMOKU_SERVER_GAME_SESSION_HPP
#define BOB_GOMOKU_SERVER_GAME_SESSION_HPP

#include <bob/database/database.hpp>
#include <bob/gomoku/message.hpp>
#include <bob/gomoku/game/board.hpp>
#include <bob/gomoku/game/replay.hpp>
#include <bob/gomoku/server/game_participant.hpp>

#include <string>
#include <queue>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

typedef unsigned int uint;

namespace bob { namespace gomoku { namespace server
{

class game_session
{
public:
    game_session
        ( game_participant_ptr player1
        , game_participant_ptr player2
        , bob::database::connection& connection
        );

    void start();

private:
    void start_game(uint curr, uint next);

    game_participant_ptr players_m[2];
    std::string          players_name_m[2];

    bob::database::connection& connection_m;
};

typedef std::shared_ptr<game_session> game_session_ptr;

inline game_session::game_session
    ( game_participant_ptr player1
    , game_participant_ptr player2
    , bob::database::connection& connection
    )
    : players_m{player1, player2}
    , connection_m(connection)
{
}

inline void game_session::start()
{
    using namespace bob::gomoku;
    uint white_i = 0;
    uint black_i = 1;

    std::srand(std::time(0));
    if (std::rand() % 2)
    {
        std::swap(white_i, black_i);
    }

    bool error = false;
    players_name_m[0] = players_m[0]->read_message(error);
    if (error) { return; }
    players_name_m[1] = players_m[1]->read_message(error);
    if (error) { return; }
    
    start_game(white_i, black_i);
    std::swap(white_i, black_i);
    start_game(white_i, black_i);
    std::swap(white_i, black_i);
    start_game(white_i, black_i);
}

inline void game_session::start_game(const uint white_i, const uint black_i)
{
    game::board board(game::board_settings(50, 50, game::player_type::white));

    // Sending the board settings.
    std::string message = std::to_string(board.settings().rows()) + " " +
                          std::to_string(board.settings().columns()) + '\n';
   
    bool error  = false;
    bool error_parse = false;
    bool over = false;

    uint player_w = 0;
    uint player_l = 0;
    std::string over_message;
    
    for (uint i = 0; i < 2; ++i)
    {
        players_m[i]->send_message(message, error);
        
        // The player disconnected or some otehr exception occured.
        if (!players_m[i]->socket().is_open() || error)
        {
            over_message = "disconnect";
            player_w = (i == 1) ? 0 : 1;
            player_l = i;
            over = true;
        }
    }

    players_m[white_i]->send_message("start\n", error);
    
    // The player disconnected or some otehr exception occured.
    if (!players_m[white_i]->socket().is_open() || error)
    {
        over_message = "disconnect";
        player_w = black_i;
        player_l = white_i;
        over = true;
    }
    
    uint curr = white_i;
    uint next = black_i;
    while (!over)
    { 
        error = false;
        error_parse = false; 
        
        message = players_m[curr]->read_message(error);
        
        // The player disconnected or some other exception occured.
        if (!players_m[curr]->socket().is_open() || error)
        {
            over_message = "disconnect";
            player_w = next;
            player_l = curr;
            break;            
        }

        bob::gomoku::message_move move = bob::gomoku::client_message::parse_move(message, error_parse);
        
        // The message was in wrong format.
        if (error_parse)
        {
            over_message = "invalid_input: \"" + message + "\"";
            player_w = next;
            player_l = curr;
            break;
        }

        players_m[next]->send_message(bob::gomoku::client_message::make_move(move), error);
        
        // The player disconnected or some other exception occured.
        if (!players_m[next]->socket().is_open() || error)
        {
            over_message = "disconnect";
            player_w = curr;
            player_l = next;
            break;            
        }
        
        // The move was invalid.
        if (!board.play(move))
        {
            over_message = "invalid_move: \"" + message + "\"";
            player_w = next;
            player_l = curr;
            break;
        }

        if (board.over())
        {
            player_w = curr;
            player_l = next;
            break;
        }
        
        std::swap(curr, next);
    }

    uint replay_id = 0;
    try
    {
        replay_id = connection_m.insert_replay
                        ( bob::database::gomoku_id
                        , bob::gomoku::game::replay_json
                            ( board.settings()
                            , board.history().begin()
                            , board.history().end()
                            , players_name_m[white_i]
                            , players_name_m[black_i]
                            )
                        );
    }
    catch(std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }

    for (uint i = 0; i < 2; ++i)
    {
        std::string res = (i == player_w) ? "w" : "l";

        if (players_m[i]->socket().is_open())
        {
            players_m[i]->send_message("over " + res + "; " + over_message + "; replay_id: " + std::to_string(replay_id) + "\n", error);
        }
    }
}

class game_session_pool
{
public:
    game_session_pool(uint thread_count);
    ~game_session_pool();
    
    void add_game_session
        ( game_participant_ptr player1
        , game_participant_ptr player2
        , bob::database::connection& connection
        );

private:
    boost::asio::io_service io_service_m;
    boost::asio::io_service::work work_m;
    boost::thread_group threads_m;
};


inline game_session_pool::game_session_pool(uint thread_count)
    : work_m(io_service_m)
{
    for (uint i = 0; i < thread_count; ++i)
    {
        threads_m.create_thread(boost::bind(&boost::asio::io_service::run, &io_service_m));
    }
}

inline game_session_pool::~game_session_pool()
{
    io_service_m.stop();
    threads_m.join_all();
}

inline void game_session_pool::add_game_session
    ( game_participant_ptr player1
    , game_participant_ptr player2
    , bob::database::connection& connection
    )
{
    game_session_ptr new_session(new game_session(player1, player2, connection));
    io_service_m.post(std::bind(&game_session::start, new_session));
}

} // namespace server
} // namespace gomoku
} // namespace bob

#endif // BOB_GOMOKU_SERVER_GAME_SESSION_HPP
