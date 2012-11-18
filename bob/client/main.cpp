#include <iostream>
#include <string>
#include <functional>

#include <bob/client/client.hpp>

inline void do_help()
{
    std::cout << "Usage: bob_client <adress> <port>" << std::endl;
}

inline void read_callback(std::string m)
{
    std::cout << m << std::endl;
}

inline void error_callback(boost::system::error_code e)
{
    std::cerr << "System error: " << e.message() << std::endl;
    std::exit(1);
}

int main(int argc, char* argv[])
{
    if (argc != 3 || (argc > 1 && std::string(argv[1]) == "--help"))
    {
        do_help();
        return  1;
    }

    try
    {
        boost::asio::io_service io_service;

        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::query query(argv[1], argv[2]);
        boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);

        bob::client client(io_service, read_callback, error_callback);
        client.connect(iterator);

        boost::thread thread(boost::bind(&boost::asio::io_service::run, &io_service));

        std::string message;
        while (std::getline(std::cin, message))
        {
            //std::cerr << "Sending: " << message << std::endl;
            message.push_back('\n');
            client.send(message);
        }

        client.disconnect();
        thread.join();
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
