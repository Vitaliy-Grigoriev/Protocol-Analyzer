// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#include <bitset>
#include <cassert>
#include <iostream>

#include "Timer.hpp"
#include "BinaryDataEngine.hpp"

using timer = analyzer::framework::diagnostic::Timer;
using namespace analyzer::framework::common::types;


int32_t main (int32_t size, char** data)
{
    auto buffer1 = "170"_u8_le.value();
    auto buffer2 = "42"_u8_be.value();
    auto buffer3 = "87"_u8_rbe.value();
    auto buffer4 = "0"_u8_le.value();
    auto buffer5 = "128"_u8_be.value();
    auto buffer6 = "255"_u8_be.value();

    assert(buffer1.To8Bit().value() == 170);
    assert(buffer2.To8Bit().value() == 42);
    assert(buffer3.To8Bit().value() == 87);
    assert(buffer4.To8Bit().value() == 0);
    assert(buffer5.To8Bit().value() == 128);
    assert(buffer6.To8Bit().value() == 255);





    /*buffer.SetDataEndianType(DATA_REVERSE_BIG_ENDIAN, true);
    std::cout << buffer.To16Bit().value() << std::endl;
    std::cout << buffer.To32Bit().value() << std::endl;*/
    return EXIT_SUCCESS;


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

    BinaryDataEngine buffer_1;
    const auto& engine_1 = buffer_1.BitsInformation();
    buffer_1.AssignData(&value_1, 1);
    std::cout << "Count 1: " << engine_1.Count() << "   " << engine_1 << std::endl;
    buffer_1.SetDataEndianType(DATA_BIG_ENDIAN);
    std::cout << "Count 1: " << engine_1.Count() << "   " << engine_1 << std::endl;
    buffer_1.SetDataEndianType(DATA_LITTLE_ENDIAN);

    BinaryDataEngine buffer_2;
    const auto& engine_2 = buffer_2.BitsInformation();
    buffer_2.AssignData(&value_2, 1);
    std::cout << "Count 2: " << engine_2.Count() << "   " << engine_2 << std::endl;

    BinaryDataEngine buffer_3;
    const auto& engine_3 = buffer_3.BitsInformation();
    buffer_3.AssignData(&value_3, 1);
    std::cout << "Count 3: " << engine_3.Count() << "   " << engine_3 << std::endl;


    std::cout << "Results:" << std::endl;


    BinaryDataEngine buffer_r1;
    const auto& engine_r1 = buffer_r1.BitsInformation();
    buffer_r1.AssignData(&value_4, 1);
    std::cout << "Count 4: " << engine_r1.Count() << "   " << engine_r1 << std::endl;

    BinaryDataEngine buffer_r2;
    const auto& engine_r2 = buffer_r2.BitsInformation();
    buffer_r2.AssignData(&value_5, 1);
    std::cout << "Count 5: " << engine_r2.Count() << "   " << engine_r2 << std::endl;

    BinaryDataEngine buffer_r3;
    const auto& engine_r3 = buffer_r3.BitsInformation();
    buffer_r3.AssignData(&value_6, 1);
    std::cout << "Count 6: " << engine_r3.Count() << "   " << engine_r3 << std::endl;

    BinaryDataEngine buffer_r4;
    const auto& engine_r4 = buffer_r4.BitsInformation();
    buffer_r4.AssignData(&value_7, 1);
    std::cout << "Count 7: " << engine_r4.Count() << "   " << engine_r4 << std::endl;

    BinaryDataEngine buffer_r5;
    const auto& engine_r5 = buffer_r5.BitsInformation();
    buffer_r5.AssignData(&value_8, 1);
    std::cout << "Count 8: " << engine_r5.Count() << "   " << engine_r5 << std::endl;

    BinaryDataEngine buffer_r6;
    const auto& engine_r6 = buffer_r6.BitsInformation();
    buffer_r6.AssignData(&value_9, 1);
    std::cout << "Count 9: " << engine_r6.Count() << "   " << engine_r6 << std::endl;


    std::cout << "XOR:" << std::endl;


    BinaryDataEngine buffer_4 = engine_1 ^ engine_2;
    const auto& engine_4 = buffer_4.BitsInformation();
    std::cout << "Count 4: " << engine_4.Count() << "   " << engine_4 << std::endl;

    BinaryDataEngine buffer_5 = engine_2 ^ engine_3;
    const auto& engine_5 = buffer_5.BitsInformation();
    std::cout << "Count 5: " << engine_5.Count() << "   " << engine_5 << std::endl;

    buffer_1.SetDataEndianType(DATA_BIG_ENDIAN);
    BinaryDataEngine buffer_6 = engine_1 ^ engine_2;
    const auto& engine_6 = buffer_6.BitsInformation();
    std::cout << "Count 4: " << engine_6.Count() << "   " << engine_6 << std::endl;
    buffer_1.SetDataEndianType(DATA_LITTLE_ENDIAN);

    buffer_2.SetDataEndianType(DATA_BIG_ENDIAN);
    BinaryDataEngine buffer_7 = engine_2 ^ engine_3;
    const auto& engine_7 = buffer_7.BitsInformation();
    std::cout << "Count 5: " << engine_7.Count() << "   " << engine_7 << std::endl;
    buffer_2.SetDataEndianType(DATA_LITTLE_ENDIAN);


    std::cout << "AND:" << std::endl;


    BinaryDataEngine buffer_8 = engine_1 & engine_2;
    const auto& engine_8 = buffer_8.BitsInformation();
    std::cout << "Count 6: " << engine_8.Count() << "   " << engine_8 << std::endl;

    BinaryDataEngine buffer_9 = engine_2 & engine_3;
    const auto& engine_9 = buffer_9.BitsInformation();
    std::cout << "Count 7: " << engine_9.Count() << "   " << engine_9 << std::endl;

    buffer_1.SetDataEndianType(DATA_BIG_ENDIAN);
    BinaryDataEngine buffer_10 = engine_1 & engine_2;
    const auto& engine_10 = buffer_10.BitsInformation();
    std::cout << "Count 6: " << engine_10.Count() << "   " << engine_10 << std::endl;
    buffer_1.SetDataEndianType(DATA_LITTLE_ENDIAN);

    buffer_2.SetDataEndianType(DATA_BIG_ENDIAN);
    BinaryDataEngine buffer_11 = engine_2 & engine_3;
    const auto& engine_11 = buffer_11.BitsInformation();
    std::cout << "Count 7: " << engine_11.Count() << "   " << engine_11 << std::endl;
    buffer_2.SetDataEndianType(DATA_LITTLE_ENDIAN);


    std::cout << "OR:" << std::endl;


    BinaryDataEngine buffer_12 = engine_1 | engine_2;
    const auto& engine_12 = buffer_12.BitsInformation();
    std::cout << "Count 8: " << engine_12.Count() << "   " << engine_12 << std::endl;

    BinaryDataEngine buffer_13 = engine_2 | engine_3;
    const auto& engine_13 = buffer_13.BitsInformation();
    std::cout << "Count 9: " << engine_13.Count() << "   " << engine_13 << std::endl;

    buffer_1.SetDataEndianType(DATA_BIG_ENDIAN);
    BinaryDataEngine buffer_14 = engine_1 | engine_2;
    const auto& engine_14 = buffer_14.BitsInformation();
    std::cout << "Count 8: " << engine_14.Count() << "   " << engine_14 << std::endl;
    buffer_1.SetDataEndianType(DATA_LITTLE_ENDIAN);

    buffer_2.SetDataEndianType(DATA_BIG_ENDIAN);
    BinaryDataEngine buffer_15 = engine_2 | engine_3;
    const auto& engine_15 = buffer_15.BitsInformation();
    std::cout << "Count 9: " << engine_15.Count() << "   " << engine_15 << std::endl;
    buffer_2.SetDataEndianType(DATA_LITTLE_ENDIAN);

    return EXIT_SUCCESS;
}
