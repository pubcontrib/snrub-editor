#include "Common.hpp"
#include <iostream>
#include <string>

void crash(std::string message)
{
    std::cerr << message << std::endl;
    exit(1);
}
