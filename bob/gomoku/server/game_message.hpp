#ifndef BOB_GOMOKU_SERVER_GAME_MESSAGE_HPP
#define BOB_GOMOKU_SERVER_GAME_MESSAGE_HPP

#include <bob/gomoku/game/board.hpp>

#include <string>
#include <sstream>

namespace bob { namespace gomoku { namespace server
{

enum class game_result_type
{
    victory
  , defeat
  , draw
};

std::ostream& operator<<(std::ostream& out, game_result_type res)
{
    switch (res)
    {
        case game_result_type::victory : { out << "victory"; break; }
        case game_result_type::defeat :  { out << "defeat"; break; }
        case game_result_type::draw :    { out << "draw"; break; }
    }

    return out;
}

struct server_message_over
{
    server_message_over(game_result_type result, std::string const& details = "")
        : result(result)
        , details(details)
    {
    }

    game_result_type result;
    std::string details;
};

typedef move_type   server_message_move;
typedef std::string server_message_over;
typedef std::string server_message_data;

namespace server_message
{
    inline std::string make_move(server_message_move const& m)
    {
        std::ostringstream out;
        out << "move " << m.x << ' ' << m.y << '\n';
        return out.str();
    }

    inline std::string make_over(server_message_over const& m)
    {
        std::ostringstream out;
        out << "over " << m.result << ' ' << m.details << '\n';
        return out.str();
    }

    inline std::string make_data(server_message_data m)
    {
        m.push_back('\n');
        return m;
    }

    inline server_message_move parse_move(std::string const& m, bool& error)
    {
        std::istringstream in(m);

        std::string command;
        client_message_move result(0, 0);
        in >> command >> m.x >> m.y;

        error = command != "move" || in.fail()
        return result;
    }
    
    inline server_message_over parse_over(std::string const& m, bool& error)
    {
        std::istringstream in(m);

        std::string command;
        client_message_move result;
        in >> command >> m.x >> m.y;

        error = command != "move" || in.fail()
        return result;
    }
    
    inline server_message_move parse_data(std::string const& m, bool& error)
    {
        std::istringstream in(m);

        std::string command;
        client_message_move result(0, 0);
        in >> command >> m.x >> m.y;

        error = command != "move" || in.fail()
        return result;
    }
};


}

} // namespace server
} // namespace gomoku
} // namespace bob

#endif // BOB_GOMOKU_SERVER_GAME_MESSAGE_HPP
