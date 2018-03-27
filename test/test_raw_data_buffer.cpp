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
    const uint16_t value_2 = 12345;
    const uint32_t value_3 = 397327479;
    const uint32_t value_4 = value_1 ^ value_2;
    const uint32_t value_5 = value_2 ^ value_3;


    RawDataBuffer buffer_1;
    const auto& engine_1 = buffer_1.BitsTransform();
    buffer_1.AssignData(&value_1, 1);
    std::cout << "Count 1: " << engine_1.Count() << "   " << engine_1 << std::endl;

    RawDataBuffer buffer_2;
    const auto& engine_2 = buffer_2.BitsTransform();
    buffer_2.AssignData(&value_2, 1);
    std::cout << "Count 2: " << engine_2.Count() << "   " << engine_2 << std::endl;

    RawDataBuffer buffer_3;
    const auto& engine_3 = buffer_3.BitsTransform();
    buffer_3.AssignData(&value_3, 1);
    std::cout << "Count 3: " << engine_3.Count() << "   " << engine_3 << std::endl;

    std::cout << std::endl;

    RawDataBuffer buffer_r1;
    const auto& engine_r1 = buffer_r1.BitsTransform();
    buffer_r1.AssignData(&value_4, 1);
    std::cout << "Count 4: " << engine_r1.Count() << "   " << engine_r1 << std::endl;

    RawDataBuffer buffer_r2;
    const auto& engine_r2 = buffer_r2.BitsTransform();
    buffer_r2.AssignData(&value_5, 1);
    std::cout << "Count 5: " << engine_r2.Count() << "   " << engine_r2 << std::endl;

    std::cout << std::endl;

    RawDataBuffer buffer_4 = engine_1 ^ engine_2;
    const auto& engine_4 = buffer_4.BitsTransform();
    std::cout << "Count 4: " << engine_4.Count() << "   " << engine_4 << std::endl;

    RawDataBuffer buffer_5 = engine_2 ^ engine_3;
    const auto& engine_5 = buffer_5.BitsTransform();
    std::cout << "Count 5: " << engine_5.Count() << "   " << engine_5 << std::endl;

    std::cout << std::endl;

    //std::cout << engine.ToShortString() << std::endl;

    return EXIT_SUCCESS;
}
