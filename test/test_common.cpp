// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#include <set>
#include <iostream>

#include "../include/framework/AnalyzerApi.hpp"


int32_t main (int32_t size, char** data)
{
    std::set<uint32_t> example;
    std::size_t count = 0;

    while (count++ < 1000000)
    {
        const auto t = analyzer::framework::common::GetRandomValue<uint32_t>();
        if (example.emplace(t).second == false) {
            std::cout << "[+] Find duplication: " << t << " after " << count << " iterations." << std::endl;
            continue;
        }
    }

    return EXIT_SUCCESS;
}
