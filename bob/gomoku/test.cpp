#include "game.hpp"
using namespace gomoku;

int main()
{
    board b(10, 10, player_type::black);
    b.play(move_type(3, 3));
    b.play(move_type(4, 3));
    b.play(move_type(2, 4));
    b.play(move_type(1, 5));
    b.play(move_type(5, 1));
    b.play(move_type(4, 1));
    b.play(move_type(4, 2));
    b.play(move_type(6, 1));
    b.play(move_type(6, 0));
    
    std::cout << b;
}
