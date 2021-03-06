#ifdef  BOB_GOMOKU_GAME_BOARD_HPP
#ifndef BOB_GOMOKU_GAME_BOARD_IMP_HPP
#define BOB_GOMOKU_GAME_BOARD_IMP_HPP

inline board_settings board::settings() const
{
    return settings_m;
}

inline std::list<move_type> const& board::history() const
{
    return history_m;
}

inline square_type board::square_at(uint x, uint y) const
{
    if (x >= settings().columns() || y >= settings().rows())
    {
        return square_type::empty;
    }
    else
    {
        return squares_m[to_index(x, y)];
    }
}

inline player_type board::current_player() const
{
    return current_player_m;
}

inline bool board::is_over() const 
{
    return over_m;
}

inline uint board::to_index(uint x, uint y) const
{
    return x + y * settings().columns();
}

inline square_type player_to_square(player_type p)
{
    switch (p)
    {
        default:
        case player_type::black: { return square_type::black; }
        case player_type::white: { return square_type::white; }
    };
}

inline player_type other_player(player_type p)
{
    switch (p)
    {
        default:
        case player_type::black: { return player_type::white; }
        case player_type::white: { return player_type::black; }
    };
}

#endif // BOB_GOMOKU_GAME_BOARD_IMP_HPP
#endif // BOB_GOMOKU_GAME_BOARD_HPP
