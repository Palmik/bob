#include "board.hpp"

#include <iterator>
#include <algorithm>

namespace bob { namespace connect { namespace game
{

inline uint uint_sub(uint x, uint y)
{
    return (x > y) ? x - y : 0;
}

board::board(board_settings const& s)
  : settings_m(s)
  , current_player_m(player_type::black)
  , over_m(false)
  , squares_m(s.columns() * s.rows(), square_type::empty)
{
}

void board::clear(board_settings const& s)
{
    settings_m = s;
    current_player_m = player_type::black;
    over_m = false;
    squares_m.assign(s.columns() * s.rows(), square_type::empty);
    history_m.clear();
}

bool board::play(move_type const& m)
{
    uint index = to_index(m.x, m.y);

    if (is_over() || m.x >= settings().columns() || m.y >= settings().rows() || squares_m[index] != square_type::empty)
    {
        return false;
    }

    squares_m[index] = player_to_square(current_player());
    history_m.push_back(m);
    check_over(m);
    
    if (!is_over() && is_new_turn())
    {
        current_player_m = other_player(current_player());
    }

    return true;
}

bool board::is_new_turn() const
{
    const uint m = history_m.size();
    return  (m == moves_init_m) ||
           ((m >  moves_init_m) && (((m - moves_init_m) % moves_turn_m) == 0));
}

bool board::check_over_neighbourhood(move_type const& m, int dx, int dy)
{
    const uint nsize = 2 * win_condition_m;
    const int cs = settings_m.columns();
    const int rs = settings_m.rows();

    uint        got  = 0;
    square_type prev = square_type::empty;

    int x = m.x;
    int y = m.y;
    for (uint i = 0; i < nsize && x >= 0 && y >= 0 && x < cs && y < rs; ++i)
    {
        square_type curr = squares_m[to_index(x, y)];

        if (curr != prev)
        {
            got = 0;
        }

        if (curr != square_type::empty)
        {
            ++got;
        }

        if (got == win_condition_m)
        {
            return true;
        }

        x += dx;
        y += dy;
        prev = curr;
    }

    return false;
}

void board::check_over(move_type const& m)
{
    // Horizontal neighbourhood.
    {
        move_type start(uint_sub(m.x, win_condition_m - 1), m.y);
        if (check_over_neighbourhood(start, 1, 0))
        {
            over_m = true;
            return;
        }
    }
    
    // Vertical neighbourhood.
    {
        move_type start(m.x, uint_sub(m.y, win_condition_m - 1));
        if (check_over_neighbourhood(start, 0, 1))
        {
            over_m = true;
            return;
        }
    }
    
    // Diagonal neighbourhood (\). The coordinate (0, 0) is top-left corner.
    {
        uint ds = std::min(win_condition_m - 1, std::min(m.x, m.y)); 
        move_type start(m.x - ds, m.y - ds);
        if (check_over_neighbourhood(start, 1, 1))
        {
            over_m = true;
            return;
        }
    }

    // Diagonal neighbourhood (/). The coordinate (0, 0) is top-left corner.
    {
        uint ds = std::min(win_condition_m - 1, std::min(settings().columns() - m.x, m.y)); 
        move_type start(m.x + ds, m.y - ds);
        if (check_over_neighbourhood(start, -1, 1))
        {
            over_m = true;
            return;
        }
    }
}

std::ostream& operator<<(std::ostream& out, square_type s)
{
    switch (s)
    {
        case square_type::empty: out << "E"; break;
        case square_type::black: out << "B"; break;
        case square_type::white: out << "W"; break;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, player_type s)
{
    switch (s)
    {
        case player_type::black: out << "B"; break;
        case player_type::white: out << "W"; break;
    }
    return out;
}

} // namespace connect
} // namespace game
} // namespace bob

