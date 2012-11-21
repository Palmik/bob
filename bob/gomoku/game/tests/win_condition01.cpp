#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE geometry/tests/core01

#include <boost/test/unit_test.hpp>

#include <bob/gomoku/game/board.hpp>
#include <bob/gomoku/game/replay.hpp>

using namespace bob::gomoku::game;

BOOST_AUTO_TEST_CASE(unit01)
{
    board b01(board_settings(10, 10, player_type::black));
    b01.play(move_type(3, 3));
    b01.play(move_type(4, 3));
    b01.play(move_type(2, 4));
    b01.play(move_type(1, 5));
    b01.play(move_type(5, 1));
    b01.play(move_type(4, 1));
    b01.play(move_type(4, 2));
    b01.play(move_type(6, 1));
    b01.play(move_type(6, 0));

    //std::cout << replay_json(b01, "aaa", "bbb") << std::endl; 
    
    BOOST_CHECK(b01.over());
    
    board b02(board_settings(10, 10, player_type::white));
    b02.play(move_type(3, 3));
    b02.play(move_type(4, 3));
    b02.play(move_type(2, 4));
    b02.play(move_type(1, 5));
    b02.play(move_type(5, 1));
    b02.play(move_type(4, 1));
    b02.play(move_type(4, 2));
    b02.play(move_type(6, 1));
    b02.play(move_type(6, 0));

    BOOST_CHECK(b02.over());
}

