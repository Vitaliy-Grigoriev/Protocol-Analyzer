// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com


#include <thread>
#include <cassert>
#include <iostream>

#include "../include/analyzer/Timer.hpp"


int main(void)
{
    analyzer::diagnostic::Timer timer(true);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::size_t count = timer.GetCount().MilliSeconds();
    std::cout << "Test 1: " << count << std::endl;
    assert(count >= 2000);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    count = timer.PauseGetCount().MilliSeconds();
    std::cout << "Test 2: " << count << std::endl;
    assert(count >= 3000);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    count = timer.GetCount().MilliSeconds();
    std::cout << "Test 3: " << count << std::endl;
    assert(count >= 3000 && count < 3100);

    timer.Start();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    count = timer.GetCount().MilliSeconds();
    std::cout << "Test 4: " << count << std::endl;
    assert(count >= 5000);

    timer.Reset(true);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    count = timer.GetCount().MilliSeconds();
    std::cout << "Test 5: " << count << std::endl;
    assert(count >= 1000 && count < 1100);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    double res = timer.PauseGetCount().Seconds();
    std::cout << "Test 6: " << res << std::endl;
    assert(count >= 2.0);

    std::cout << "Test success." << std::endl;
    return 0;
}
