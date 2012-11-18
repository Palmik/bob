#ifndef BOB_GOMOKU_MESSAGE_HPP
#define BOB_GOMOKU_MESSAGE_HPP

#include <string>
#include <sstream>

namespace bob { namespace gomoku
{

typedef std::string message_authentication;
typedef move_type   message_move;

namespace client_message
{
    inline std::string make_authentication(message_authentication const& m)
    {
        std::ostringstream out;
        out << "authentication " << m << '\n';
        return out.str();
    }

    inline std::string make_move(message_move const& m)
    {
        std::ostringstream out;
        out << m.x << " " << m.y << '\n';
        return out.str();
    }

    inline message_authentication parse_authentication(std::string const& m, bool& error)
    {
        std::istringstream in(m);
        
        std::string command;
        message_authentication result;
        in >> command >> result;

        error = command != "authentication" || in.fail();
        return result;
    }

    inline message_move parse_move(std::string const& m, bool& error)
    {
        std::istringstream in(m);

        message_move result(0, 0);
        in >> m.x >> m.y;

        error = in.fail()
        return result;
    }

} // namespace client_message

} // namespace gomoku
} // namespace bob

#endif // BOB_GOMOKU_MESSAGE_HPP
