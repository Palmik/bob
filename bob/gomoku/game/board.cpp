#include "board.hpp"

namespace bob { namespace gomoku { namespace game
{

board::board(size_type columns, size_type rows, player_type first_player)
  : columns_m(columns)
  , rows_m(rows)
  , current_player_m(first_player)
  , first_player_m(first_player)
  , over_m(false)
  , squares_m(columns * rows)
{
    std::fill(squares_m.begin(), squares_m.end(), square_type::empty); 
}

std::ostream& operator<<(std::ostream& out, player_type p)
{
    switch (p)
    {
        case player_type::white :
        {
            out << 'W';
            break;
        }
        case player_type::black :
        {
            out << 'B';  
            break;
        }
    }

    return out;
}

std::ostream& operator<<(std::ostream& out, square_type s)
{
    switch (s)
    {
        case square_type::empty :
        {
            out << '.';
            break;
        }
        case square_type::white :
        {
            out << 'W';
            break;
        }
        case square_type::black :
        {
            out << 'B';
            break;
        }
    }

    return out;
}

std::ostream& operator<<(std::ostream& out, move_type const& m)
{
    out << m.x << ' ' << m.y;
    return out;
}

std::ostream& operator<<(std::ostream& out, board const& b)
{
    std::string border_horizontal(b.columns() + 2, '#');
    
    out << border_horizontal << '\n';
    for (board::size_type y = 0; y < b.rows(); ++y)
    {
        out << '#';
        for (board::size_type x = 0; x < b.columns(); ++x)
        {
            out << b.square_at(x, y);
        }
        out << "#\n";
    } 
    out << border_horizontal << "\n\n";

    out << "HISTORY: \n";
    
    player_type curr = b.first_player();
    for (move_type const& m : b.history())
    {
        out << curr << ": " << m << '\n';
        curr = other_player(curr);
    }
    out << std::endl;
    if (b.over())
    {
        out << "WINNER: " << b.current_player() << std::endl;
    }

    return out;
}

bool board::play(move_type const& m)
{
    size_type index = to_index(m.x, m.y);

    if (over() || m.x >= columns() || m.y >= rows() || squares_m[index] != square_type::empty)
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
        size_type e = std::min(m.x + (need - 1), columns() - 1);
        
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
        size_type e = std::min(m.y + (need - 1), rows() - 1);
        
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
        int de =  std::min(need - 1, std::min(columns() - m.x, rows() - m.y));

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
        int ds = -std::min(need - 1, std::min(m.x, rows() - m.y));
        int de =  std::min(need - 1, std::min(columns() - m.x, m.y));

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
