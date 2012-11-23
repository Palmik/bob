#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE geometry/tests/core01

#include <boost/test/unit_test.hpp>

#include <bob/connect/game/board.hpp>
#include <bob/connect/game/replay.hpp>

using namespace bob::connect::game;

BOOST_AUTO_TEST_CASE(unit01)
{
    board b01(board_settings(10, 10));
    BOOST_CHECK_EQUAL(b01.current_player(), player_type::black);
    b01.play(move_type(0, 0));
    
    BOOST_CHECK_EQUAL(b01.current_player(), player_type::white);
    b01.play(move_type(7, 0));
    BOOST_CHECK_EQUAL(b01.current_player(), player_type::white);
    b01.play(move_type(6, 1));

    BOOST_CHECK_EQUAL(b01.current_player(), player_type::black);
    b01.play(move_type(0, 1));
    BOOST_CHECK_EQUAL(b01.current_player(), player_type::black);
    b01.play(move_type(0, 2));
    
    BOOST_CHECK_EQUAL(b01.current_player(), player_type::white);
    b01.play(move_type(5, 2));
    BOOST_CHECK_EQUAL(b01.current_player(), player_type::white);
    b01.play(move_type(4, 3));

    BOOST_CHECK_EQUAL(b01.current_player(), player_type::black);
    b01.play(move_type(0, 3));
    BOOST_CHECK_EQUAL(b01.current_player(), player_type::black);
    b01.play(move_type(0, 4));
    
    BOOST_CHECK_EQUAL(b01.current_player(), player_type::white);
    b01.play(move_type(3, 4));
    BOOST_CHECK_EQUAL(b01.current_player(), player_type::white);
    b01.play(move_type(2, 5));
    BOOST_CHECK_EQUAL(b01.current_player(), player_type::white);
  
    BOOST_CHECK(b01.is_over());
}

