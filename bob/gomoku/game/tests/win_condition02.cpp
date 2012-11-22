#include <bob/gomoku/game/board.hpp>
#include <bob/gomoku/game/replay.hpp>

using namespace bob::gomoku::game;

int main()
{
    board b01(board_settings(10, 10));
    b01.play(move_type(3, 3));
    b01.play(move_type(4, 3));
    b01.play(move_type(2, 4));
    b01.play(move_type(1, 5));
    b01.play(move_type(5, 1));
    b01.play(move_type(4, 1));
    b01.play(move_type(4, 2));
    b01.play(move_type(6, 1));
    b01.play(move_type(6, 0));

    std::cout << b01.over() << std::endl;
    
    board b02(board_settings(10, 10));
    b02.play(move_type(0, 1));
    b02.play(move_type(3, 2));
    b02.play(move_type(0, 2));
    b02.play(move_type(5, 2));
    b02.play(move_type(0, 3));
    b02.play(move_type(7, 2));
    b02.play(move_type(0, 4));
    b02.play(move_type(5, 5));
    b02.play(move_type(0, 0));

    std::cout << b02.over() << std::endl;
}



