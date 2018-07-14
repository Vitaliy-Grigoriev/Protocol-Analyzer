// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#include <thread>
#include <cassert>
#include <iostream>

#include "../include/framework/Timer.hpp"


int32_t main (int32_t size, char** data)
{
    analyzer::diagnostic::Timer timer(true);
    // Test 1.  Work 2 seconds.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::size_t count = timer.GetCount().MilliSeconds();
    assert(count >= 2000 && count <= 2005);
    std::cout << "[+] Test 1:  " << count << std::endl;

    // Test 2.  Work plus 1 second.
    std::this_thread::sleep_for(std::chrono::seconds(1));
    count = timer.PauseAndGetCount().MilliSeconds();
    assert(count >= 3000 && count <= 3005);
    std::cout << "[+] Test 2:  " << count << std::endl;

    // Test 3.  Not work.
    std::this_thread::sleep_for(std::chrono::seconds(1));
    count = timer.GetCount().MilliSeconds();
    assert(count >= 3000 && count <= 3005);
    std::cout << "[+] Test 3:  " << count << std::endl;

    // Test 4.  Work plus 2 seconds.
    timer.Start();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    count = timer.GetCount().MilliSeconds();
    assert(count >= 5000 && count <= 5005);
    std::cout << "[+] Test 4:  " << count << std::endl;

    // Test 5.  Reset and work 1 second.
    timer.Reset(true);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    count = timer.GetCount().MicroSeconds();
    assert(count >= 1000000 && count <= 1050000);
    std::cout << "[+] Test 5:  " << count << std::endl;

    // Test 6.  Work plus 1 second.
    std::this_thread::sleep_for(std::chrono::seconds(1));
    const double res = timer.PauseAndGetCount().Seconds();
    assert(res >= 2.0 && res <= 2.05);
    std::cout << "[+] Test 6:  " << res << std::endl;

    // Compare tests.
    analyzer::diagnostic::Timer timer_1(true);
    std::this_thread::sleep_for(std::chrono::microseconds(15));
    analyzer::diagnostic::Timer timer_2(true);

    // Test 7.  Compare after 1 second.
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto count_1 = timer_1.GetCount();
    auto count_2 = timer_2.GetCount();
    assert(count_1 == count_2);
    std::cout << "[+] Test 7:  " << count_1.MicroSeconds() << "   " << count_2.MicroSeconds() << std::endl;


    std::cout << std::endl << "[+] Test success." << std::endl;
    return EXIT_SUCCESS;
}
