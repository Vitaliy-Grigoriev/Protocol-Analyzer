// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <mutex>
#include <iostream>

#include "../include/analyzer/Api.hpp"


int32_t main (int32_t size, char** data)
{
    std::mutex mutex1 = { }, mutex2 = { };

    mutex2.lock();
    try {
        std::scoped_lock<std::mutex, std::mutex> lock { mutex1, mutex2 };
    }
    catch (const std::system_error& err) {
        std::cerr << "[error] Error: " << err.what() << std::endl;
    }

    std::cerr << "[-] Exit." << std::endl;
    return EXIT_SUCCESS;
}


