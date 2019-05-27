// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#include "../../include/framework/IntegerValue.hpp"


namespace analyzer::framework::common::types
{
    // Assign constructor of IntegerValue class.
    IntegerValue::IntegerValue (const INTEGER_DIMENSION dim, const bool isPositiveInteger) noexcept
        : storedValue(BinaryDataEngine(dim / 8, DATA_MODE_DEFAULT, DATA_SYSTEM_ENDIAN)), dimension(dim), sign(isPositiveInteger)
    { }

    // 8-bit unsigned integer constructor of IntegerValue class.
    IntegerValue::IntegerValue (uint8_t value) noexcept
        : storedValue(BinaryDataEngine(sizeof(uint8_t), DATA_MODE_DEFAULT, DATA_SYSTEM_ENDIAN)), dimension(INTEGER_8BIT_VALUE), sign(true)
    {
        storedValue.AssignData(&value, 1);
    }

    // 16-bit unsigned integer constructor of IntegerValue class.
    IntegerValue::IntegerValue (uint16_t value) noexcept
        : storedValue(BinaryDataEngine(sizeof(uint16_t), DATA_MODE_DEFAULT, DATA_SYSTEM_ENDIAN)), dimension(INTEGER_16BIT_VALUE), sign(true)
    {
        storedValue.AssignData(&value, 1);
    }

    // 32-bit unsigned integer constructor of IntegerValue class.
    IntegerValue::IntegerValue (uint32_t value) noexcept
            : storedValue(BinaryDataEngine(sizeof(uint32_t), DATA_MODE_DEFAULT, DATA_SYSTEM_ENDIAN)), dimension(INTEGER_32BIT_VALUE), sign(true)
    {
        storedValue.AssignData(&value, 1);
    }

    // 64-bit unsigned integer constructor of IntegerValue class.
    IntegerValue::IntegerValue (uint64_t value) noexcept
            : storedValue(BinaryDataEngine(sizeof(uint64_t), DATA_MODE_DEFAULT, DATA_SYSTEM_ENDIAN)), dimension(INTEGER_64BIT_VALUE), sign(true)
    {
        storedValue.AssignData(&value, 1);
    }

    // 8-bit integer constructor of IntegerValue class.
    IntegerValue::IntegerValue (int8_t value) noexcept
        : storedValue(BinaryDataEngine(sizeof(int8_t), DATA_MODE_DEFAULT, DATA_SYSTEM_ENDIAN)), dimension(INTEGER_8BIT_VALUE), sign(false)
    {
        storedValue.AssignData(&value, 1);
    }

    // 16-bit integer constructor of IntegerValue class.
    IntegerValue::IntegerValue (int16_t value) noexcept
        : storedValue(BinaryDataEngine(sizeof(int16_t), DATA_MODE_DEFAULT, DATA_SYSTEM_ENDIAN)), dimension(INTEGER_16BIT_VALUE), sign(false)
    {
        storedValue.AssignData(&value, 1);
    }

    // 32-bit integer constructor of IntegerValue class.
    IntegerValue::IntegerValue (int32_t value) noexcept
        : storedValue(BinaryDataEngine(sizeof(int32_t), DATA_MODE_DEFAULT, DATA_SYSTEM_ENDIAN)), dimension(INTEGER_32BIT_VALUE), sign(false)
    {
        storedValue.AssignData(&value, 1);
    }

    // 64-bit integer constructor of IntegerValue class.
    IntegerValue::IntegerValue (int64_t value) noexcept
        : storedValue(BinaryDataEngine(sizeof(int64_t), DATA_MODE_DEFAULT, DATA_SYSTEM_ENDIAN)), dimension(INTEGER_64BIT_VALUE), sign(false)
    {
        storedValue.AssignData(&value, 1);
    }


}  // namespace types.
