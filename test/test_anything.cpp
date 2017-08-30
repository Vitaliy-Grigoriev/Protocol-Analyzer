// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>

#include "../include/analyzer/Api.hpp"


int main(void)
{
    std::allocator<std::size_t> alloc;
    std::cout << alloc.max_size() << std::endl;

    return EXIT_SUCCESS;
}

