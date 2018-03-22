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
    //buffer.SetDataEndianType(analyzer::common::types::DATA_BIG_ENDIAN);
    buffer.SetDataModeType(analyzer::common::types::DATA_MODE_INDEPENDENT);
    std::cout << engine << std::endl;
    std::cout << std::endl;

    //buffer.SetDataEndianType(analyzer::common::types::DATA_LITTLE_ENDIAN);
    buffer.SetDataModeType(analyzer::common::types::DATA_MODE_DEPENDENT);
    std::bitset<32> raw(value_1);
    std::cout << raw.to_string() << std::endl;
    std::cout << engine << std::endl;

    std::cout << std::endl;

    engine.Set(31).Set(23).Set(18, false).Set(0);
    raw.set(31).set(23).set(18, false).set(0);
    std::cout << raw.to_string() << std::endl;
    std::cout << engine << std::endl;

    std::cout << std::endl;

    engine.ShiftLeft(5).ShiftRight(17);
    raw <<= 5; raw >>= 17;
    std::cout << raw.to_string() << std::endl;
    std::cout << engine << std::endl;

    std::cout << std::endl;

    engine.InvertBlock();
    raw.flip();
    std::cout << "Count: " << raw.count() << "   " << raw.to_string() << std::endl;
    std::cout << "Count: " << engine.Count() << "   " << engine << std::endl;

    std::cout << std::endl;

    engine.InvertBlock();
    raw.flip();
    std::cout << "Count: " << raw.count() << "   " << raw.to_string() << std::endl;
    std::cout << "Count: " << engine.Count() << "   " << engine << std::endl;

    std::cout << std::endl;

    engine.RoundShiftLeft(5);
    std::cout << engine << std::endl;

    engine.RoundShiftRight(5);
    std::cout << engine << std::endl;

    std::cout << std::endl;


    /*RawDataBuffer buffer;
    buffer.AssignData<uint32_t>(&value_1, 1);
    for (std::size_t idx = 0; idx < buffer.BitsTransform().Length(); ++idx) {
        std::cout << buffer.BitsTransform().Test(idx);
    }
    std::cout << std::endl;
    std::bitset<32> raw(value_1);
    std::cout << raw.to_string() << std::endl;*/

    /*std::bitset<8> raw_1(0);
    raw_1.set(1);
    std::cout << raw_1.to_string() << std::endl;*/
    /*buffer.AssignData(&value_2, 1);
    std::cout << buffer.BitsTransform() << std::endl;

    buffer.AssignData(&value_3, 1);
    std::cout << buffer.BitsTransform() << std::endl;*/

    return EXIT_SUCCESS;
}
