#ifndef BOB_GOMOKU_SERVER_GAME_SESSION_HPP
#define BOB_GOMOKU_SERVER_GAME_SESSION_HPP

#include <string>
#include <queue>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include <bob/database/database.hpp>

#include <bob/connect/game/board.hpp>
#include <bob/connect/server/game_participant.hpp>

typedef unsigned int uint;

namespace bob { namespace connect { namespace server
{

namespace detail
{
    struct game_participant_data
    {
    public:
        game_participant_data
            ( game_participant_ptr c
            , std::string const& n
            , uint i
            , uint r
            )
            : connection(c)
            , name(n)
            , id(i)
            , result(r)
        {
        }

        game_participant_ptr connection;
        std::string name;
        uint id;
        uint result;
    };

} // namespace detail

static const uint result_draw = 0;
static const uint result_loss = 1;
static const uint result_win = 2;

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
        ( detail::game_participant_data* player_w
        , detail::game_participant_data* player_l
        , std::string const& info
        , bool replay = true
        );

    uint save_game_session();
    void authenticate();

    detail::game_participant_data player_black_m;
    detail::game_participant_data player_white_m;

    bob::connect::game::board_settings board_settings_m;
    bob::connect::game::board board_m;

    bob::database::connection& connection_db_m;
};

typedef std::shared_ptr<game_session> game_session_ptr;

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

} // namespace server
} // namespace connect
} // namespace bob

#endif // BOB_GOMOKU_SERVER_GAME_SESSION_HPP
