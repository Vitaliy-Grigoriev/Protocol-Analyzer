// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#include <bitset>
#include <iostream>

#include "../include/analyzer/AnalyzerApi.hpp"

using analyzer::common::types::RawDataBuffer;


int32_t main (int32_t size, char** data)
{
    /*struct test
    {
        uint32_t a;
        float b;
        double c;
        int16_t d;
    } value;

    RawDataBuffer buffer;
    buffer.AssignData<struct test>(&value, 1);*/

    const uint16_t value_0 = 37988;
    const uint32_t value_1 = 573475684;
    const uint16_t value_2 = 12423;
    const uint32_t value_3 = value_1 & value_2;

    RawDataBuffer buffer;
    const auto& engine = buffer.BitsTransform();
    buffer.AssignData(&value_1, 1);
    buffer.SetDataModeType(analyzer::common::types::DATA_MODE_INDEPENDENT);
    std::cout << "Count 2: " << engine.Count() << "   " << engine << std::endl;
    std::cout << std::endl;

    buffer.SetDataModeType(analyzer::common::types::DATA_MODE_DEPENDENT);
    std::bitset<32> raw(value_1);
    std::cout << "Count 1: " << raw.count() << "   " << raw.to_string() << std::endl;
    std::cout << "Count 2: " << engine.Count() << "   " << engine << std::endl;

    std::cout << std::endl;

    std::cout << "Count 3: " << engine.Count(11, 18) << "   " << engine.ToString(11, 18) << std::endl;
    std::cout << "Count 2: " << engine.Count() << "   " << engine.Reverse(11, 18) << std::endl;
    std::cout << "Count 4: " << engine.Count(11, 18) << "   " << engine.ToString(11, 18) << std::endl;
    std::cout << "Count 2: " << engine.Count() << "   " << engine.Reverse(11, 18) << std::endl;

    std::cout << std::endl;

    std::cout << engine.GetFirstIndex() << std::endl;
    std::cout << engine.GetLastIndex() << std::endl;
    std::cout << engine.GetFirstIndex(11, 16) << std::endl;
    std::cout << engine.GetLastIndex(11, 16) << std::endl;
    std::cout << engine.GetFirstIndex(11, 16, false) << std::endl;
    std::cout << engine.GetLastIndex(11, 16, false) << std::endl;

    std::cout << std::endl;

    RawDataBuffer result = engine << 15;
    std::cout << "Count 5: " << result.BitsTransform().Count() << "   " << result.BitsTransform() << std::endl;

    //std::cout << engine.ToShortString() << std::endl;

    return EXIT_SUCCESS;
}
