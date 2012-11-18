#ifndef BOB_GOMOKU_SERVER_GAME_SESSION_HPP
#define BOB_GOMOKU_SERVER_GAME_SESSION_HPP

#include "../game/board.hpp"
#include "../message.hpp"
#include "game_participant.hpp"

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
        );

    void start();

private:
    uint start_game(uint curr, uint next, bob::gomoku::game::board& board);

    game_participant_ptr players_m[2];
};

typedef std::shared_ptr<game_session> game_session_ptr;

inline game_session::game_session
    ( game_participant_ptr player1
    , game_participant_ptr player2
    )
    : players_m{player1, player2}
{
}

inline void game_session::start()
{
    uint curr = 0;
    uint next = 1;

    std::srand(std::time(0));
    if (std::rand() % 2)
    {
        std::swap(curr, next);
    }
    
    std::string message;

    // Authentication phase.
    message = players_m[0]->read_message();
    message = players_m[1]->read_message();

    {
        bob::gomoku::game::board board(100, 100);
        start_game(curr, next, board);
    }
    
    std::swap(curr, next);
    
    {
        bob::gomoku::game::board board(100, 100);
        start_game(curr, next, board);
    }
}

inline uint game_session::start_game(uint curr, uint next, bob::gomoku::game::board& board)
{
    // Sending the board settings.
    std::string message = std::to_string(board.rows()) + " " + std::to_string(board.columns()) + '\n';
    
    players_m[0]->send_message(message);
    players_m[1]->send_message(message);

    players_m[curr]->send_message("start\n");
   
    bool error = false;
    while (true)
    {
        message = players_m[curr]->read_message();
        bob::gomoku::message_move move = bob::gomoku::client_message::parse_move(message, error);
        
        if (error)
        {
            players_m[curr]->send_message("over d invalid_input: \"" + message + "\"\n");
            players_m[next]->send_message("over w invalid_input: \"" + message + "\"\n");
            return next;
        }

        players_m[next]->send_message(bob::gomoku::client_message::make_move(move));
        board.play(move);

        if (board.over())
        {
            players_m[curr]->send_message("over w\n");
            players_m[next]->send_message("over d\n");
            return curr;
        }
        
        std::swap(curr, next);
    }
}

class game_session_pool
{
public:
    game_session_pool(uint thread_count);
    ~game_session_pool();
    
    void add_game_session(game_participant_ptr player1, game_participant_ptr player2);

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

inline void game_session_pool::add_game_session(game_participant_ptr player1, game_participant_ptr player2)
{
    game_session_ptr new_session(new game_session(player1, player2));
    io_service_m.post(std::bind(&game_session::start, new_session));
}

} // namespace server
} // namespace gomoku
} // namespace bob

#endif // BOB_GOMOKU_SERVER_GAME_SESSION_HPP
