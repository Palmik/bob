#include "game_server.hpp"

#include <string>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

using namespace bob::connect::server;

inline void do_help()
{
    std::cout << "Usage: bob_connect_server <port>" << std::endl;
}

inline void error_callback(boost::system::error_code e)
{
    std::cerr << "System error: " << e.message() << std::endl;
    std::exit(1);
}

int main(int argc, char* argv[])
{
    if (argc != 2 || (argc > 1 && std::string(argv[1]) == "--help"))
    {
        do_help();
        return  1;
    }

    try
    {
        boost::asio::io_service io_service;
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), std::atoi(argv[1]));
        
        game_server server(io_service, endpoint, error_callback);
        server.start();

        io_service.run();
    }
    catch (boost::system::system_error const& e)
    {
        std::cerr << "System error: " << e.what() << std::endl;
        return 1;
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown error" << std::endl;
        return 1;
    }
}
