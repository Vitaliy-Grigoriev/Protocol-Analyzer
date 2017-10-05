// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>

#include "../include/analyzer/Api.hpp"
#include "../include/analyzer/Common.hpp"

using namespace analyzer::common;


int32_t main (int32_t size, char** data)
{
    const std::size_t value_1 = 10;
    std::cout << getHexValue(value_1) << std::endl;

    const std::size_t value_2 = 923876706450976023;
    std::cout << getHexValue(value_2, sizeof(std::size_t)) << std::endl;

    const unsigned char value_3[3] = { 0, 4, 15 };
    std::cout << getHexString(value_3, sizeof(value_3)) << std::endl;

    return EXIT_SUCCESS;
}

