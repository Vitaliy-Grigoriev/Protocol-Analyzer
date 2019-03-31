// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#include <bitset>
#include <cassert>
#include <iostream>

#include "../include/framework/Timer.hpp"
#include "FrameworkApi.hpp"

using timer = analyzer::framework::diagnostic::Timer;
using namespace analyzer::framework::common::types;

#pragma pack(push, 1)
struct Data
{
    uint32_t SequenceNumber;
    uint32_t AcknowledgmentNumber;
    uint8_t  Offset_Reserved_NS;
    uint8_t  CWR_ECE_TcpFlags;
    uint16_t WindowSize;
    uint16_t Checksum;
    uint16_t UrgentPointer;
} const tcp { 0xF0E0D0C0, 0x0C0D0E0F, 0xE5, 0x92, 0x7468, 0x5CD5, 0xAAAA };
#pragma pack(pop)

const uint16_t byte_pattern[7] = { 4, 4, 1, 1, 2, 2, 2 };
const uint16_t bit_pattern[9] = { 32, 32, 4, 3, 3, 6, 16, 16, 16 };


int32_t main (int32_t size, char** data)
{
    BinaryStructuredDataEngine buffer1;
    const bool res = buffer1.Constructor().AssignStructuredData<DATA_BIG_ENDIAN>(&tcp, 1, byte_pattern, 7, true);
    assert(res == true);

    const BinaryDataEngine binData(reinterpret_cast<const std::byte*>(&tcp), sizeof(tcp), DATA_BIG_ENDIAN);
    ConstantBinaryStructuredDataEngine buffer(binData, byte_pattern, sizeof(byte_pattern) / sizeof(uint16_t));
    const auto bitCount = buffer.Data().BitsInformation().Count();

    timer Timer1(true);
    for (uint16_t idx = 0; idx < 100; ++idx) {
        buffer.GetNumericalField<uint16_t>(5).value();
    }
    std::cout << Timer1.GetCount().NanoSeconds() / 100 << std::endl;
    Timer1.Reset(true);
    for (uint16_t idx = 0; idx < 100; ++idx) {
        buffer.GetFieldByReference(5).value().To16Bit().value();
    }
    std::cout << Timer1.PauseAndGetCount().NanoSeconds() / 100 << std::endl;


    assert(buffer.GetNumericalField<uint16_t>(4).value() == 0x6874);
    assert(buffer.GetNumericalField<uint16_t>(5).value() == 0xD55C);

    ConstantBinaryStructuredDataEngine res1 = buffer ^ buffer1;
    assert(res1.Data().BitsInformation().Any() == false && res1.Data().Size() == buffer.Data().Size());

    buffer = std::move(res1);
    assert(buffer.Data().BitsInformation().Count() == 0 && res1.Data().BitsInformation().Count() == bitCount);
    res1 = std::move(buffer);
    assert(buffer.Data().BitsInformation().Count() == bitCount && res1.Data().BitsInformation().Count() == 0);

    assert(binData == true && buffer == true);
    std::cout << buffer.Data().ToHexString() << std::endl;
    std::cout << buffer.ToFormattedString() << std::endl;

    assert(buffer.GetField(0).value().To32Bit().value() == 3234914544);
    assert(buffer.GetField(1).value().To32Bit().value() == 252579084);
    assert(buffer.GetField(2).value().To8Bit().value() == 229);
    assert(buffer.GetField(3).value().To8Bit().value() == 146);
    assert(buffer.GetField(4).value().To16Bit().value() == 26740);  // 29800le.
    assert(buffer.GetField(5).value().To16Bit().value() == 54620);  // 23765le.
    assert(buffer.GetField(6).value().To16Bit().value() == 43690);

    const uint16_t offset = buffer.GetSubField<uint16_t, DATA_MODE_INDEPENDENT>(2, 0, 4).value();
    assert(offset == 14);
    assert(buffer.GetFieldBit<DATA_MODE_INDEPENDENT>(2, 0) == true);
    assert(buffer.GetFieldBit<DATA_MODE_INDEPENDENT>(2, 1) == true);
    assert(buffer.GetFieldBit<DATA_MODE_INDEPENDENT>(2, 2) == true);
    assert(buffer.GetFieldBit<DATA_MODE_INDEPENDENT>(2, 3) == false);
    assert(buffer.GetFieldBit<DATA_MODE_INDEPENDENT>(2, 4) == false);
    assert(buffer.GetFieldBit<DATA_MODE_INDEPENDENT>(2, 5) == true);
    assert(buffer.GetFieldBit<DATA_MODE_INDEPENDENT>(2, 6) == false);
    assert(buffer.GetFieldBit<DATA_MODE_INDEPENDENT>(2, 7) == true);

    const uint16_t flags = buffer.GetSubField<uint16_t, DATA_MODE_INDEPENDENT>(3, 2, 6).value();
    assert(flags == 18);
    assert(buffer.GetFieldBit<DATA_MODE_INDEPENDENT>(3, 0) == true);
    assert(buffer.GetFieldBit<DATA_MODE_INDEPENDENT>(3, 1) == false);
    assert(buffer.GetFieldBit<DATA_MODE_INDEPENDENT>(3, 2) == false);
    assert(buffer.GetFieldBit<DATA_MODE_INDEPENDENT>(3, 3) == true);
    assert(buffer.GetFieldBit<DATA_MODE_INDEPENDENT>(3, 4) == false);
    assert(buffer.GetFieldBit<DATA_MODE_INDEPENDENT>(3, 5) == false);
    assert(buffer.GetFieldBit<DATA_MODE_INDEPENDENT>(3, 6) == true);
    assert(buffer.GetFieldBit<DATA_MODE_INDEPENDENT>(3, 7) == false);


    timer Timer(true);
    for (uint16_t idx = 0; idx < 100; ++idx) {
        buffer.GetNonemptyFieldIndex(0, bit_pattern, 9).value();
    }
    std::cout << Timer.GetCount().MicroSeconds() / 100 << std::endl;
    for (uint16_t idx = 0; idx < 100; ++idx) {
        buffer.GetNonemptyFieldIndex(1, bit_pattern, 9).value();
    }
    std::cout << Timer.UpdateAndGetCount().MicroSeconds() / 100 << std::endl;
    for (uint16_t idx = 0; idx < 100; ++idx) {
        buffer.GetNonemptyFieldIndex(2, bit_pattern, 9).value();
    }
    std::cout << Timer.UpdateAndGetCount().MicroSeconds() / 100  << std::endl;
    for (uint16_t idx = 0; idx < 100; ++idx) {
        buffer.GetNonemptyFieldIndex(3, bit_pattern, 9).value();
    }
    std::cout << Timer.UpdateAndGetCount().MicroSeconds() / 100  << std::endl;
    for (uint16_t idx = 0; idx < 100; ++idx) {
        buffer.GetNonemptyFieldIndex(4, bit_pattern, 9).value();
    }
    std::cout << Timer.UpdateAndGetCount().MicroSeconds() / 100  << std::endl;
    for (uint16_t idx = 0; idx < 100; ++idx) {
        buffer.GetNonemptyFieldIndex(5, bit_pattern, 9).value();
    }
    std::cout << Timer.UpdateAndGetCount().MicroSeconds() / 100  << std::endl;
    for (uint16_t idx = 0; idx < 100; ++idx) {
        buffer.GetNonemptyFieldIndex(6, bit_pattern, 9).value();
    }
    std::cout << Timer.UpdateAndGetCount().MicroSeconds() / 100  << std::endl;
    for (uint16_t idx = 0; idx < 100; ++idx) {
        buffer.GetNonemptyFieldIndex(7, bit_pattern, 9).value();
    }
    std::cout << Timer.UpdateAndGetCount().MicroSeconds() / 100  << std::endl;
    for (uint16_t idx = 0; idx < 100; ++idx) {
        buffer.GetNonemptyFieldIndex(8, bit_pattern, 9).value();
    }
    std::cout << Timer.PauseAndGetCount().MicroSeconds() / 100  << std::endl;


    return EXIT_SUCCESS;
}
