#ifndef BOB_GOMOKU_GAME_BOARD_HPP
#define BOB_GOMOKU_GAME_BOARD_HPP

#include <vector>
#include <list>
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace bob { namespace gomoku { namespace game
{

enum class player_type : uint
{
    black
  , white
};

enum class square_type : uint
{
    empty
  , black
  , white
};

struct move_type
{
public:
    move_type(std::size_t x, std::size_t y) : x(x), y(y) {}
    std::size_t x;
    std::size_t y;
};

struct board_settings
{
public:
    board_settings(std::size_t columns, std::size_t rows)
        : columns_m(columns)
        , rows_m(rows)
    {
    }


    std::size_t columns() const { return columns_m; }
    std::size_t rows() const { return rows_m; }

private:
    std::size_t columns_m;
    std::size_t rows_m;
};

class board
{
public:
    typedef std::size_t size_type;
    
    board(board_settings const& s);

    board_settings settings() const;
    
    void clear(board_settings const& s);

    square_type square_at(size_type x, size_type y) const;

    player_type current_player() const;

    bool play(move_type const& m);
    bool over() const;

    std::list<move_type> const& history() const;

private:
    void check_over(move_type const& m);
    size_type to_index(size_type x, size_type y) const;
   
    board_settings settings_m;

    player_type current_player_m;

    bool over_m;

    std::vector<square_type> squares_m;
    std::list<move_type> history_m;
};

player_type other_player(player_type p);
square_type player_to_square(player_type p);

#include "board.imp.hpp"

} // namespace game
} // namespace gomoku
} // namespace bob

#endif
