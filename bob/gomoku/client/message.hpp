#ifndef BOB_GOMOKU_CLIENT_MESSAGE_HPP
#define BOB_GOMOKU_CLIENT_MESSAGE_HPP

#include <string>
#include <sstream>

// This is basically:
//
// data ClientMessage = ClientMessageAuthentication Authentication
//                    | ClientMessageMove           Move
//
// But in C++, so it's ridiculously long.

namespace bob { namespace gomoku { namespace client
{

typedef std::string client_message_authentication;
typedef move_type   client_message_move;

namespace client_message
{
    inline std::string make_authentication(client_message_authentication const& m)
    {
        std::ostringstream out;
        out << "authentication " << m << '\n';
        return out.str();
    }

    inline std::string make_move(client_message_move const& m)
    {
        std::ostringstream out;
        out << m.x << " " << m.y << '\n';
        return out.str();
    }

    inline client_message_authentication parse_authentication(std::string const& m, bool& error)
    {
        std::istringstream in(m);
        
        std::string command;
        client_message_authentication result;
        in >> command >> result;

        error = command != "authentication" || in.fail();
        return result;
    }

    inline client_message_move parse_move(std::string const& m, bool& error)
    {
        std::istringstream in(m);

        client_message_move result(0, 0);
        in >> m.x >> m.y;

        error = in.fail()
        return result;
    }

} // namespace client_message

/*
namespace detail
{
    enum class client_message_type : unsigned int
    {
        authentication
      , move
    };

    union client_message_union
    {
        client_message_union(client_message_authentication const& authentication)
            : authentication(authentication)
        {
        }

        client_message_union(client_message_move const& move)
            : move(move)
        {
        }

        client_message_authentication authentication;
        client_message_move           move;
    };

} // namespace detail

class client_message
{
public:
    client_message make_authentication(client_message_authentication const&);
    client_message make_move(client_message_move const&);
    
    std::string to_string() const;

    client_message_authentication to_authentication(bool& error) const;
    client_message_move           to_move(bool& error) const;

private:
    explicit client_message(client_message_authentication const& a);
    explicit client_message(client_message_move const& m);
    
    detail::client_message_union union_m;

    client_message_type type_m;
};

client_message client_message::make_authentication(client_message_authentication const& a)
{
    return client_message(a);
}

client_message client_message::make_move(client_message_move const& m)
{
    return client_message(m);
}

std::string to_string() const
{
    std::ostringstrem out;
    switch (type_m)
    {
        case client_message_type::authentication :
        {
            out << "authentication " << union_m.authentication << '\n';
        }
        case client_message_type::move :
        {
            out << "move " << union_m.move.x << " " << union_m.move.y << '\n';
        }
    }

    return out.str();
}

client_message_authentication to_authentication(bool& error) const
{
    error = type_m == client_message_type::authentication;
    return union_m.authentication;
}

client_message_move to_move(bool& error) const
{
    error = type_m == client_message_type::move;
    return union_m.move;
}

client_message::client_message(client_message_authentication const& a)
    : union_m(a)
    , type_m(client_message_type::authentication)
{
}

client_message::client_message(client_message_move const& m)
    : union_m(m)
    , type_m(client_message_type::move)
{
}

*/

} // namespace client
} // namespace gomoku
} // namespace bob

#endif // BOB_GOMOKU_CLIENT_MESSAGE_HPP
