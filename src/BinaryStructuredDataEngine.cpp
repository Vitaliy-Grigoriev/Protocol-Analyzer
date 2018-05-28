// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#include "../include/analyzer/BinaryStructuredDataEngine.hpp"


namespace analyzer::common::types
{
    /* ****************************************************************************************************** */
    /* ************************************* BinaryStructuredDataEngine ************************************* */

    // Copy assignment constructor of BinaryStructuredDataEngine class.
    BinaryStructuredDataEngine::BinaryStructuredDataEngine (const BinaryStructuredDataEngine& other) noexcept
    {
        if (other.data.Data() != nullptr)
        {
            data = other.data;
            if (data.Data() != nullptr)
            {
                fieldsCount = other.fieldsCount;
                dataPattern = system::allocMemoryForArray<uint16_t>(fieldsCount, other.dataPattern.get(), fieldsCount);
                dataEndianType = other.dataEndianType;
            }
        }
    }

    // Move assignment constructor of BinaryStructuredDataEngine class.
    BinaryStructuredDataEngine::BinaryStructuredDataEngine (BinaryStructuredDataEngine&& other) noexcept
    {
        if (other.data.Data() != nullptr)
        {
            data = std::move(other.data);
            fieldsCount = other.fieldsCount;
            dataPattern = std::move(other.dataPattern);
            dataEndianType = other.dataEndianType;
            other.Clear();
        }
    }

    // Method that changes endian type of stored data in BinaryStructuredDataEngine class.
    void BinaryStructuredDataEngine::SetDataEndianType (const DATA_ENDIAN_TYPE endian) noexcept
    {
        if (dataEndianType == endian) { return; }
        dataEndianType = endian;

        std::size_t block = 0;
        for (std::size_t idx = 0; idx < fieldsCount; ++idx)
        {
            if (dataPattern[idx] == 1) {
                block++;
                continue;
            }

            for (std::size_t jdx = 0; jdx < dataPattern[idx] / 2; ++jdx)
            {
                *data.GetAt(block + jdx) ^= *data.GetAt(block + dataPattern[idx] - jdx - 1);
                *data.GetAt(block + dataPattern[idx] - jdx - 1) ^= *data.GetAt(block + jdx);
                *data.GetAt(block + jdx) ^= *data.GetAt(block + dataPattern[idx] - jdx - 1);
            }
            block += dataPattern[idx];
        }
    }

    // Method that sets new bit value to the selected field bit offset of structured data.
    bool BinaryStructuredDataEngine::SetFieldBit (const uint16_t fieldIndex, const uint16_t offset, const bool value) noexcept
    {
        if (fieldIndex >= fieldsCount || offset >= dataPattern[fieldIndex] * 8) {
            return false;
        }

        // Calculate common bit offset to start bit in selected field.
        std::size_t bitOffset = static_cast<std::size_t>(std::accumulate(dataPattern.get(), dataPattern.get() + fieldIndex, 0) * 8);
        // Calculate common bit offset to selected bit in selected field.
        bitOffset += offset;
        data.BitsTransform().Set(bitOffset, value);
        return true;
    }

    // Method that clears the data.
    void BinaryStructuredDataEngine::Clear(void) noexcept
    {
        data.Clear();
        fieldsCount = 0;
        dataPattern.reset(nullptr);
        dataEndianType = BinaryDataEngine::system_endian;
    }

    // Method that outputs internal binary structured data by pattern in string format.
    std::string BinaryStructuredDataEngine::ToString(void) const noexcept
    {
        std::ostringstream result;
        if (data.IsEmpty() == false)
        {
            result.unsetf(std::ios_base::boolalpha);
            uint16_t patternBlock = 0, bitCount = 0;

            result << "\nField 1:   ";

            if (dataEndianType == DATA_BIG_ENDIAN)
            {
                for (std::size_t idx = 0; idx < data.BitsTransform().Length(); ++idx)
                {
                    if (idx % 8 == 0 && bitCount != 0) { result << ' '; }
                    result << data.BitsTransform().Test(idx);
                    if (++bitCount == dataPattern[patternBlock] * 8) {
                        if (++patternBlock == fieldsCount) { break; }
                        result << "\nField " << patternBlock + 1 << ":   ";
                        bitCount = 0;
                    }
                }
            }
            else  // If data endian type is DATA_LITTLE_ENDIAN.
            {
                std::size_t commonBitCount = 0;  // Count of all outputted bits.
                std::size_t bitOffset = 0;  // Start bit offset in each bit field.
                // Iterate over all fields of structured data.
                for (uint16_t field = 0; field < fieldsCount; ++field)
                {
                    // Select required byte offset in each field.
                    for (int32_t byteOffset = dataPattern[field] - 1; byteOffset >= 0; --byteOffset)
                    {
                        // Calculate bit offset to start bit in selected byte.
                        const std::size_t offset = bitOffset + static_cast<uint32_t>(byteOffset * 8);
                        // Iterate over all bits in selected byte.
                        for (std::size_t idx = offset; idx < offset + 8; ++idx)
                        {
                            if (commonBitCount++ % 8 == 0 && bitCount != 0) { result << ' '; }
                            result << data.BitsTransform().Test(idx);
                            if (++bitCount == dataPattern[patternBlock] * 8) {
                                if (++patternBlock == fieldsCount) { break; }
                                result << "\nField " << patternBlock + 1 << ":   ";
                                bitCount = 0;
                            }
                        }
                    }
                    bitOffset += dataPattern[field] * 8;
                }
            }
        }
        return result.str();
    }



    // Copy assignment operator of BinaryStructuredDataEngine class.
    BinaryStructuredDataEngine& BinaryStructuredDataEngine::operator= (const BinaryStructuredDataEngine& other) noexcept
    {
        if (this != &other && other.data.Data() != nullptr)
        {
            data = other.data;
            if (data.Data() != nullptr)
            {
                fieldsCount = other.fieldsCount;
                dataPattern = system::allocMemoryForArray<uint16_t>(fieldsCount, other.dataPattern.get(), fieldsCount);
                dataEndianType = other.dataEndianType;
            }
        }
        return *this;
    }

    // Move assignment operator of BinaryStructuredDataEngine class.
    BinaryStructuredDataEngine& BinaryStructuredDataEngine::operator= (BinaryStructuredDataEngine&& other) noexcept
    {
        if (this != &other && other.data.Data() != nullptr)
        {
            data = std::move(other.data);
            fieldsCount = other.fieldsCount;
            dataPattern = std::move(other.dataPattern);
            dataEndianType = other.dataEndianType;
            other.Clear();
        }
        return *this;
    }

}  // namespace types.
