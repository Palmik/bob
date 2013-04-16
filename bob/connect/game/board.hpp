#ifndef BOB_GOMOKU_GAME_BOARD_HPP
#define BOB_GOMOKU_GAME_BOARD_HPP

#include <vector>
#include <list>
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace bob { namespace connect { namespace game
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
    move_type(uint x, uint y) : x(x), y(y) {}
    uint x;
    uint y;
};

struct board_settings
{
public:
    board_settings(uint columns, uint rows)
        : columns_m(columns)
        , rows_m(rows)
    {
    }


    uint columns() const { return columns_m; }
    uint rows() const { return rows_m; }

private:
    uint columns_m;
    uint rows_m;
};

class board
{
public:
    board(board_settings const& s);
    void clear(board_settings const& s);

    board_settings settings() const;
    
    square_type square_at(uint x, uint y) const;
    player_type current_player() const;

    bool play(move_type const& m);
    bool is_over() const;
    bool is_new_turn() const;

    std::list<move_type> const& history() const;

private:
    void check_over(move_type const& m);
    bool check_over_neighbourhood(move_type const& m, int dx, int dy);
    uint to_index(uint x, uint y) const;

    board_settings settings_m;
    
    player_type current_player_m;
    bool over_m;

    std::vector<square_type> squares_m;
    std::list<move_type>     history_m;

    static const uint moves_init_m = 1;
    static const uint moves_turn_m = 2;
    static const uint win_condition_m = 6;
};

player_type other_player(player_type p);
square_type player_to_square(player_type p);

std::ostream& operator<<(std::ostream& out, square_type s);
std::ostream& operator<<(std::ostream& out, player_type p);

#include "board.imp.hpp"

} // namespace game
} // namespace connect
} // namespace bob

#endif
