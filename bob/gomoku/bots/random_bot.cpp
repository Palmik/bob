#include <iostream>
#include <vector>
#include <sstream>
#include <ctime>
#include <cstdlib>

typedef unsigned int uint;

std::vector<bool> board;
std::size_t board_w = 0;
std::size_t board_h = 0;
std::size_t board_free = 0;

inline void play_random()
{
    if (board_free > 10)
    {
        uint index_rand = std::rand() % board_free;
        uint index = 0;
    
        for (uint y = 0; y < board_h; ++y)
        {
            for (uint x = 0; x < board_w; ++x)
            {
                if (!board.at(x + y*board_w))
                {
                    if (index == index_rand)
                    {
                        std::cout << x << " " << y << "\n";
                        board[x + y*board_w] = true;
                        --board_free;
                        return;
                    }
                    ++index;
                }
            }
        }
    }
    else
    {
        std::cout << "0 0\n";
    }

}

inline bool is_start_line(std::string const& line)
{
    return line == "start";
}

inline bool is_over_line(std::string const& line)
{
    char const* over_string = "over";
    char const* line_string = line.c_str();

    while (*over_string && *line_string)
    {
        if (*over_string != *line_string)
        {
            return false;
        }
        ++over_string;
        ++line_string;
    }

    return true;
}

inline void play()
{
    std::cin >> board_w >> board_h >> std::ws;
    board_free = board_w * board_h;
    board.assign(board_w * board_h, false);

    std::string line;
    getline(std::cin, line, '\n');

    if (line == "black")
    {
        play_random();
    }

    while (getline(std::cin, line, '\n'))
    {
        if (line == "") { continue; }
        if (is_over_line(line))
        {
            return;
        }

        std::istringstream in(line);
        uint x;
        uint y;
        in >> x >> y;

        board[x + y * board_w] = true;
        --board_free;

        play_random();
    }
}

int main()
{
    std::srand(std::time(0));

    std::cout << "random_bot\n";

    //std::cerr << is_over_line("over awdwd") << is_over_line("over") << is_over_line("oveaawd") << std::endl;
    play();
    play();
    play();
}

