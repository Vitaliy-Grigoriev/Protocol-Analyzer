// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>

#include "../include/analyzer/AnalyzerApi.hpp"


int main(void)
{
    std::set<uint32_t> example;
    std::size_t count = 0;

    while (count++ < 1000000)
    {
        const auto t = analyzer::common::GetRandomValue<uint32_t>();
        if (example.emplace(t).second == false) {
            std::cout << "[+] Find duplication: " << t << " after " << count << " iterations." << std::endl;
            continue;
        }
    }

    return EXIT_SUCCESS;
}

