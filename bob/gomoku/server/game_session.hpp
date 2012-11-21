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
        ( game_participant_ptr player_white
        , game_participant_ptr player_black
        , bob::database::connection& connection
        );

    void start();

private:
    void start_game();
    void report_draw(std::string const& info, bool replay = true);
    void report_over
        ( game_participant_ptr player_w
        , game_participant_ptr player_l
        , std::string const& info
        , bool replay = true
        );

    uint save_replay();

    game_participant_ptr player_white_m;
    game_participant_ptr player_black_m;

    std::string player_white_name_m;
    std::string player_black_name_m;

    bob::gomoku::game::board_settings board_settings_m;
    bob::gomoku::game::board board_m;

    bob::database::connection& connection_db_m;
};

typedef std::shared_ptr<game_session> game_session_ptr;

inline game_session::game_session
    ( game_participant_ptr player_white
    , game_participant_ptr player_black
    , bob::database::connection& connection
    )
    : player_white_m(player_white)
    , player_black_m(player_black)
    , player_white_name_m("<unknown>")
    , player_black_name_m("<unknown>")
    , board_settings_m(50, 50, bob::gomoku::game::player_type::white)
    , board_m(board_settings_m)
    , connection_db_m(connection)
{
}

inline void game_session::start()
{
    using namespace bob::gomoku;

    std::srand(std::time(0));
    if (std::rand() % 2)
    {
        std::swap(player_white_m, player_black_m);
    }

    try
    {
        player_white_name_m = player_white_m->read_message();
        player_black_name_m = player_black_m->read_message();
    }
    catch(...)
    {
        report_draw("oponnent_disconnect");
    }

    try
    { 
        std::cerr << "aa: " << board_m.over() << std::endl;
        start_game();
    
        std::swap(player_white_m, player_black_m);
        std::swap(player_white_name_m, player_black_name_m);
        board_m.clear(board_settings_m);
        

        std::cerr << "aa: " << board_m.over() << std::endl;
        start_game();
    
        std::swap(player_white_m, player_black_m);
        std::swap(player_white_name_m, player_black_name_m);
        board_m.clear(board_settings_m);
        
        std::cerr << "aa: " << board_m.over() << std::endl;
        start_game();

        std::swap(player_white_m, player_black_m);
        std::swap(player_white_name_m, player_black_name_m);
        board_m.clear(board_settings_m);
    }
    catch(...)
    {
        report_draw("oponnent_disconnect");
    }
}


inline void game_session::start_game()
{
    std::string message = std::to_string(board_m.settings().rows()) + " " +
                          std::to_string(board_m.settings().columns()) + '\n';

    // Sending the board settings.
    player_white_m->send_message(message);
    player_black_m->send_message(message);

    // Sending the start signal.
    player_white_m->send_message("start\n");
    
    bool error_parse = false;
    
    game_participant_ptr curr = player_white_m;
    game_participant_ptr next = player_black_m;
    while (true)
    { 
        error_parse = false; 
        
        message = curr->read_message();
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
        next->send_message(bob::gomoku::client_message::make_move(move));
        
        std::swap(curr, next);
    }
}

inline void game_session::report_draw(std::string const& info, bool replay)
{
    std::string replay_info;
    if (replay)
    {
        replay_info = "; replay_id: " + std::to_string(save_replay());
    }
    
    try
    {
        player_white_m->send_message("over d; " + info + replay_info + "\n");
        player_black_m->send_message("over d; " + info + replay_info + "\n");
    }
    catch(...)
    {
    }
}

inline void game_session::report_over
    ( game_participant_ptr player_w
    , game_participant_ptr player_l
    , std::string const& info
    , bool replay
    )
{
    std::string replay_info;
    if (replay)
    {
        replay_info = "; replay_id: " + std::to_string(save_replay());
    }
    
    try
    {
        player_l->send_message("over l; " + info + replay_info + "\n");
        player_w->send_message("over w; " + info + replay_info + "\n");
    }
    catch(...)
    {
    }
}

inline uint game_session::save_replay()
{
    uint replay_id = 0;
    try
    {
        replay_id = connection_db_m.insert_replay
                        ( bob::database::gomoku_id
                        , bob::gomoku::game::replay_json
                            ( board_m
                            , player_white_name_m
                            , player_black_name_m
                            )
                        );
    }
    catch(std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return replay_id;
}

class game_session_pool
{
public:
    game_session_pool(uint thread_count);
    ~game_session_pool();
    
    void add_game_session
        ( game_participant_ptr player_white
        , game_participant_ptr player_black
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

#endif // BOB_GOMOKU_SERVER_GAME_SESSION_HPP
