#include "board.hpp"

namespace bob { namespace gomoku { namespace game
{

board::board(board_settings const& s)
  : settings_m(s)
  , current_player_m(s.first_player())
  , over_m(false)
  , squares_m(s.columns() * s.rows())
{
    std::fill(squares_m.begin(), squares_m.end(), square_type::empty); 
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
    if (n.empty())
    {
        return false;
    }

    const board::size_type need = 5;
          board::size_type got  = 0;
    
    // First iteration.
    square_type prev = n.front();
    if (prev != square_type::empty)
    {
        ++got;
    }

    for (square_type const& curr : n)
    {
        if (got == need)
        {
            return true;
        }

        if (curr == prev)
        {
            if (curr != square_type::empty)
            {
                ++got;
            }
        }
        else
        {
            got = 0;
        }

        prev = curr;
    }

    return false;
}

void board::check_over(move_type const& m)
{
    const size_type need = 5;

    std::vector<square_type> neighbourhood;
    neighbourhood.reserve(2 * need);

    // Horizontal neighbourhood.
    {
        neighbourhood.clear();
        size_type s = std::max(m.x - (need - 1), 0ul);
        size_type e = std::min(m.x + (need - 1), settings().columns() - 1);
        
        for (size_type i = s; i <= e; ++i)
        {
            neighbourhood.push_back(squares_m[to_index(i, m.y)]);
        }

        if (check_neighbourhood(neighbourhood))
        {
            over_m = true;
            return;
        }
    }
    
    // Vertical neighbourhood.
    {
        neighbourhood.clear();
        size_type s = std::max(m.y - (need - 1), 0ul);
        size_type e = std::min(m.y + (need - 1), settings().rows() - 1);
        
        for (size_type i = s; i <= e; ++i)
        {
            neighbourhood.push_back(squares_m[to_index(m.x, i)]);
        }

        if (check_neighbourhood(neighbourhood))
        {
            over_m = true;
            return;
        }
    }
    
    // Diagonal neighbourhood (\). The coordinate (0, 0) is top-left corner.
    {
        neighbourhood.clear();
        int ds = -std::min(need - 1, std::min(m.x, m.y));
        int de =  std::min(need - 1, std::min(settings().columns() - m.x, settings().rows() - m.y));

        for (int d = ds; d <= de; ++d)
        {
            neighbourhood.push_back(squares_m[to_index(m.x + d, m.y + d)]);
        }

        if (check_neighbourhood(neighbourhood))
        {
            over_m = true;
            return;
        }
    }

    // Diagonal neighbourhood (/). The coordinate (0, 0) is top-left corner.
    {
        neighbourhood.clear();
        int ds = -std::min(need - 1, std::min(m.x, settings().rows() - m.y));
        int de =  std::min(need - 1, std::min(settings().columns() - m.x, m.y));

        for (int d = ds; d <= de; ++d)
        {
            neighbourhood.push_back(squares_m[to_index(m.x + d, m.y - d)]);
        }

        if (check_neighbourhood(neighbourhood))
        {
            over_m = true;
            return;
        }
    }
}

} // namespace gomoku
} // namespace game
} // namespace bob
