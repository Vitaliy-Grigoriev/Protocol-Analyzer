// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#include <bitset>
#include <iostream>

#include "../include/analyzer/AnalyzerApi.hpp"

namespace types = analyzer::common::types;
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
    const uint16_t value_2 = 12345;
    const uint32_t value_3 = 397327479;
    const uint32_t value_4 = value_1 ^ value_2;
    const uint32_t value_5 = value_2 ^ value_3;
    const uint32_t value_6 = value_1 & value_2;
    const uint32_t value_7 = value_2 & value_3;
    const uint32_t value_8 = value_1 | value_2;
    const uint32_t value_9 = value_2 | value_3;

    RawDataBuffer buffer_1;
    const auto& engine_1 = buffer_1.BitsTransform();
    buffer_1.AssignData(&value_1, 1);
    std::cout << "Count 1: " << engine_1.Count() << "   " << engine_1 << std::endl;
    buffer_1.SetDataEndianType(types::DATA_BIG_ENDIAN);
    std::cout << "Count 1: " << engine_1.Count() << "   " << engine_1 << std::endl;
    buffer_1.SetDataEndianType(types::DATA_LITTLE_ENDIAN);

    RawDataBuffer buffer_2;
    const auto& engine_2 = buffer_2.BitsTransform();
    buffer_2.AssignData(&value_2, 1);
    std::cout << "Count 2: " << engine_2.Count() << "   " << engine_2 << std::endl;

    RawDataBuffer buffer_3;
    const auto& engine_3 = buffer_3.BitsTransform();
    buffer_3.AssignData(&value_3, 1);
    std::cout << "Count 3: " << engine_3.Count() << "   " << engine_3 << std::endl;


    std::cout << "Results:" << std::endl;


    RawDataBuffer buffer_r1;
    const auto& engine_r1 = buffer_r1.BitsTransform();
    buffer_r1.AssignData(&value_4, 1);
    std::cout << "Count 4: " << engine_r1.Count() << "   " << engine_r1 << std::endl;

    RawDataBuffer buffer_r2;
    const auto& engine_r2 = buffer_r2.BitsTransform();
    buffer_r2.AssignData(&value_5, 1);
    std::cout << "Count 5: " << engine_r2.Count() << "   " << engine_r2 << std::endl;

    RawDataBuffer buffer_r3;
    const auto& engine_r3 = buffer_r3.BitsTransform();
    buffer_r3.AssignData(&value_6, 1);
    std::cout << "Count 6: " << engine_r3.Count() << "   " << engine_r3 << std::endl;

    RawDataBuffer buffer_r4;
    const auto& engine_r4 = buffer_r4.BitsTransform();
    buffer_r4.AssignData(&value_7, 1);
    std::cout << "Count 7: " << engine_r4.Count() << "   " << engine_r4 << std::endl;

    RawDataBuffer buffer_r5;
    const auto& engine_r5 = buffer_r5.BitsTransform();
    buffer_r5.AssignData(&value_8, 1);
    std::cout << "Count 8: " << engine_r5.Count() << "   " << engine_r5 << std::endl;

    RawDataBuffer buffer_r6;
    const auto& engine_r6 = buffer_r6.BitsTransform();
    buffer_r6.AssignData(&value_9, 1);
    std::cout << "Count 9: " << engine_r6.Count() << "   " << engine_r6 << std::endl;


    std::cout << "XOR:" << std::endl;


    RawDataBuffer buffer_4 = engine_1 ^ engine_2;
    const auto& engine_4 = buffer_4.BitsTransform();
    std::cout << "Count 4: " << engine_4.Count() << "   " << engine_4 << std::endl;

    RawDataBuffer buffer_5 = engine_2 ^ engine_3;
    const auto& engine_5 = buffer_5.BitsTransform();
    std::cout << "Count 5: " << engine_5.Count() << "   " << engine_5 << std::endl;

    buffer_1.SetDataEndianType(types::DATA_BIG_ENDIAN);
    RawDataBuffer buffer_6 = engine_1 ^ engine_2;
    const auto& engine_6 = buffer_6.BitsTransform();
    std::cout << "Count 4: " << engine_6.Count() << "   " << engine_6 << std::endl;
    buffer_1.SetDataEndianType(types::DATA_LITTLE_ENDIAN);

    buffer_2.SetDataEndianType(types::DATA_BIG_ENDIAN);
    RawDataBuffer buffer_7 = engine_2 ^ engine_3;
    const auto& engine_7 = buffer_7.BitsTransform();
    std::cout << "Count 5: " << engine_7.Count() << "   " << engine_7 << std::endl;
    buffer_2.SetDataEndianType(types::DATA_LITTLE_ENDIAN);


    std::cout << "AND:" << std::endl;


    RawDataBuffer buffer_8 = engine_1 & engine_2;
    const auto& engine_8 = buffer_8.BitsTransform();
    std::cout << "Count 6: " << engine_8.Count() << "   " << engine_8 << std::endl;

    RawDataBuffer buffer_9 = engine_2 & engine_3;
    const auto& engine_9 = buffer_9.BitsTransform();
    std::cout << "Count 7: " << engine_9.Count() << "   " << engine_9 << std::endl;

    buffer_1.SetDataEndianType(types::DATA_BIG_ENDIAN);
    RawDataBuffer buffer_10 = engine_1 & engine_2;
    const auto& engine_10 = buffer_10.BitsTransform();
    std::cout << "Count 6: " << engine_10.Count() << "   " << engine_10 << std::endl;
    buffer_1.SetDataEndianType(types::DATA_LITTLE_ENDIAN);

    buffer_2.SetDataEndianType(types::DATA_BIG_ENDIAN);
    RawDataBuffer buffer_11 = engine_2 & engine_3;
    const auto& engine_11 = buffer_11.BitsTransform();
    std::cout << "Count 7: " << engine_11.Count() << "   " << engine_11 << std::endl;
    buffer_2.SetDataEndianType(types::DATA_LITTLE_ENDIAN);


    std::cout << "OR:" << std::endl;


    RawDataBuffer buffer_12 = engine_1 | engine_2;
    const auto& engine_12 = buffer_12.BitsTransform();
    std::cout << "Count 8: " << engine_12.Count() << "   " << engine_12 << std::endl;

    RawDataBuffer buffer_13 = engine_2 | engine_3;
    const auto& engine_13 = buffer_13.BitsTransform();
    std::cout << "Count 9: " << engine_13.Count() << "   " << engine_13 << std::endl;

    buffer_1.SetDataEndianType(types::DATA_BIG_ENDIAN);
    RawDataBuffer buffer_14 = engine_1 | engine_2;
    const auto& engine_14 = buffer_14.BitsTransform();
    std::cout << "Count 8: " << engine_14.Count() << "   " << engine_14 << std::endl;
    buffer_1.SetDataEndianType(types::DATA_LITTLE_ENDIAN);

    buffer_2.SetDataEndianType(types::DATA_BIG_ENDIAN);
    RawDataBuffer buffer_15 = engine_2 | engine_3;
    const auto& engine_15 = buffer_15.BitsTransform();
    std::cout << "Count 9: " << engine_15.Count() << "   " << engine_15 << std::endl;
    buffer_2.SetDataEndianType(types::DATA_LITTLE_ENDIAN);


    //std::cout << engine.ToShortString() << std::endl;

    return EXIT_SUCCESS;
}
