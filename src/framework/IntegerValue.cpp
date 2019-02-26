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

    // Reference 8-bit unsigned integer constructor of IntegerValue class.
    IntegerValue::IntegerValue (uint8_t& value) noexcept
        : storedValue(BinaryDataEngine(sizeof(uint8_t), DATA_MODE_DEFAULT, DATA_SYSTEM_ENDIAN)), dimension(INTEGER_8BIT_VALUE), sign(true)
    { }


}  // namespace types.
