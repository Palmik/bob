#include "board.hpp"

#include <iterator>
#include <algorithm>

namespace bob { namespace gomoku { namespace game
{

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
    size_type index = to_index(m.x, m.y);

    if (over() || m.x >= settings().columns() || m.y >= settings().rows() || squares_m[index] != square_type::empty)
    {
        return false;
    }

    squares_m[index] = player_to_square(current_player());
    history_m.push_back(m);
    check_over(m);
    
    if (!over())
    {
        current_player_m = other_player(current_player());
    }

    return true;
}

bool check_neighbourhood(std::vector<square_type> const& n)
{
    //std::copy(n.begin(), n.end(), std::ostream_iterator<square_type>(std::cerr, ""));
    //std::cerr << std::endl;
    
    if (n.empty())
    {
        return false;
    }

    const board::size_type need = 5;
          board::size_type got  = 0;
    
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
        
        if (got == need)
        {
            return true;
        }

        prev = curr;
    }

    return false;
}

void board::check_over(move_type const& m)
{
    const int need = 5;

    std::vector<square_type> neighbourhood;
    neighbourhood.reserve(2 * need);

    // Horizontal neighbourhood.
    {
        neighbourhood.clear();
        int s = std::max(static_cast<int>(m.x) - (need - 1), 0);
        int e = std::min(static_cast<int>(m.x) + (need - 1), static_cast<int>(settings().columns()) - 1);

        for (int i = s; i <= e; ++i)
        {
            neighbourhood.push_back(squares_m[to_index(i, m.y)]);
        }

        //std::cerr << "n -: ";
        if (check_neighbourhood(neighbourhood))
        {
            over_m = true;
            return;
        }
    }
    
    // Vertical neighbourhood.
    {
        neighbourhood.clear();
        size_type s = std::max(static_cast<int>(m.y) - (need - 1), 0);
        size_type e = std::min(static_cast<int>(m.y) + (need - 1), static_cast<int>(settings().rows()) - 1);
        
        for (size_type i = s; i <= e; ++i)
        {
            neighbourhood.push_back(squares_m[to_index(m.x, i)]);
        }

        //std::cerr << "n |: ";
        if (check_neighbourhood(neighbourhood))
        {
            over_m = true;
            return;
        }
    }
    
    // Diagonal neighbourhood (\). The coordinate (0, 0) is top-left corner.
    {
        neighbourhood.clear();
        int ds = -std::min(need - 1, static_cast<int>(std::min(m.x, m.y)));
        int de =  std::min(need - 1, static_cast<int>(std::min(settings().columns() - m.x, settings().rows() - m.y)));

        for (int d = ds; d <= de; ++d)
        {
            neighbourhood.push_back(squares_m[to_index(m.x + d, m.y + d)]);
        }

        //std::cerr << "n \\: ";
        if (check_neighbourhood(neighbourhood))
        {
            over_m = true;
            return;
        }
    }

    // Diagonal neighbourhood (/). The coordinate (0, 0) is top-left corner.
    {
        neighbourhood.clear();
        int ds = -std::min(need - 1, static_cast<int>(std::min(m.x, settings().rows() - m.y)));
        int de =  std::min(need - 1, static_cast<int>(std::min(settings().columns() - m.x, m.y)));

        for (int d = ds; d <= de; ++d)
        {
            neighbourhood.push_back(squares_m[to_index(m.x + d, m.y - d)]);
        }

        //std::cerr << "n /: ";
        if (check_neighbourhood(neighbourhood))
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

} // namespace gomoku
} // namespace game
} // namespace bob
