// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <mutex>
#include <iostream>

#include "../include/analyzer/Api.hpp"
#include "../include/analyzer/LockedDeque.hpp"

using namespace analyzer;


int32_t main (int32_t size, char** data)
{
    /*std::mutex mutex1 = { }, mutex2 = { };

    mutex2.lock();
    try {
        std::scoped_lock<std::mutex, std::mutex> lock { mutex1, mutex2 };
    }
    catch (const std::system_error& err) {
        std::cerr << "[error] Error: " << err.what() << std::endl;
    }*/

    uint32_t res1, res2;
    common::types::LockedDeque<uint32_t> deque1, deque2;
    deque1.Push(32);
    deque2.Swap(deque1);

    std::cout << std::boolalpha << deque1.PopBack(res1) << "  " << res1 << std::endl;

    deque2.PopBack(res2);
    std::cout << res2 << std::endl;

    deque2.Push(33);
    std::deque<uint32_t> res3;
    std::cout << std::boolalpha << deque2.Move(res3) << "  " << *res3.begin() << std::endl;
    std::cout << std::boolalpha << deque2.IsEmpty() << std::endl;

    deque2.Push(34);
    deque2.Swap(deque2);
    deque2.PopBack(res2);
    std::cout << res2 << std::endl;


    std::cerr << "[+] Exit." << std::endl;
    return EXIT_SUCCESS;
}


