#ifndef BOB_UTILITY_HPP
#define BOB_UTILITY_HPP

#include <string>
#include <sstream>

template <typename T>
std::string to_string(T const& x)
{
    std::ostringstream out;
    out << x;
    return out.str();
}

#endif // BOB_UTILITY_HPP

