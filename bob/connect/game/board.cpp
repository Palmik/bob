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

bool check_neighbourhood(std::vector<square_type> const& n, uint win_condition)
{
    //std::copy(n.begin(), n.end(), std::ostream_iterator<square_type>(std::cerr, ""));
    //std::cerr << std::endl;
    
    if (n.empty())
    {
        return false;
    }

    uint got  = 0;
    square_type prev = square_type::empty;

    for (square_type const& curr : n)
    {
        if (curr == prev || prev == square_type::empty)
        {
            if (curr != square_type::empty)
            {
                ++got;
            }
        }
        else
        {
            if (curr == square_type::empty)
            {
                got = 0;
            }
            else
            {
                got = 1;
            }
        }
        
        if (got == win_condition)
        {
            return true;
        }

        prev = curr;
    }

    return false;
}

void board::check_over(move_type const& m)
{
    std::vector<square_type> neighbourhood;
    neighbourhood.reserve(2 * win_condition_m);

    // Horizontal neighbourhood.
    {
        neighbourhood.clear();
        int s = uint_sub(m.x, (win_condition_m - 1));
        int e = std::min(m.x + (win_condition_m - 1), settings().columns() - 1);

        for (int i = s; i <= e; ++i)
        {
            neighbourhood.push_back(squares_m[to_index(i, m.y)]);
        }

        //std::cerr << "n -: ";
        if (check_neighbourhood(neighbourhood, win_condition_m))
        {
            over_m = true;
            return;
        }
    }
    
    // Vertical neighbourhood.
    {
        neighbourhood.clear();
        uint s = uint_sub(m.y, (win_condition_m - 1));
        uint e = std::min(m.y + (win_condition_m - 1), settings().rows() - 1);
        
        for (uint i = s; i <= e; ++i)
        {
            neighbourhood.push_back(squares_m[to_index(m.x, i)]);
        }

        //std::cerr << "n |: ";
        if (check_neighbourhood(neighbourhood, win_condition_m))
        {
            over_m = true;
            return;
        }
    }
    
    // Diagonal neighbourhood (\). The coordinate (0, 0) is top-left corner.
    {
        neighbourhood.clear();
        int ds = -std::min(win_condition_m - 1, std::min(m.x, m.y));
        int de =  std::min(win_condition_m - 1, std::min(settings().columns() - m.x, settings().rows() - m.y));

        for (int d = ds; d <= de; ++d)
        {
            neighbourhood.push_back(squares_m[to_index(m.x + d, m.y + d)]);
        }

        //std::cerr << "n \\: ";
        if (check_neighbourhood(neighbourhood, win_condition_m))
        {
            over_m = true;
            return;
        }
    }

    // Diagonal neighbourhood (/). The coordinate (0, 0) is top-left corner.
    {
        neighbourhood.clear();
        int ds = -std::min(win_condition_m - 1, std::min(m.x, settings().rows() - m.y));
        int de =  std::min(win_condition_m - 1, std::min(settings().columns() - m.x, m.y));

        for (int d = ds; d <= de; ++d)
        {
            neighbourhood.push_back(squares_m[to_index(m.x + d, m.y - d)]);
        }

        //std::cerr << "n /: ";
        if (check_neighbourhood(neighbourhood, win_condition_m))
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
