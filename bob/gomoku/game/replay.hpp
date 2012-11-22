#ifndef BOB_GOMOKU_GAME_REPLAY_HPP
#define BOB_GOMOKU_GAME_REPLAY_HPP

#include <bob/gomoku/game/board.hpp>

namespace bob { namespace gomoku { namespace game 
{

inline std::string replay_data(board const& b)
{
    std::string result;
    
    result += "{ ";
    result += " \"settings\": { \"columns\": " + std::to_string(b.settings().columns()) +
                              ", \"rows\": "    + std::to_string(b.settings().rows()) +
                              " } ";

    std::string moves_string;
    moves_string += "[ "; 
    for (auto it = b.history().begin(); it != b.history().end(); ++it)
    {
        if (it != b.history().begin()) { moves_string += ", "; }
        moves_string += " { \"x\": " + std::to_string(it->x) + ", \"y\": " + std::to_string(it->y) + " } ";
    }
    moves_string += " ]";

    result += ", \"moves\": " + moves_string;
    result += " }";

    return result;
}

} // namespace game
} // namespace gomoku
} // namespace bob

#endif // BOB_GOMOKU_GAME_REPLAY_HPP
