// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#include "../../include/framework/BinaryStructuredDataEngine.hpp"


namespace analyzer::common::types
{
    // Constructor of BinaryStructuredDataEngine class with prepared structured data.
    BinaryStructuredDataEngine::BinaryStructuredDataEngine (BinaryDataEngine& input, const uint16_t* pattern, const uint16_t size, const DATA_ENDIAN_TYPE endian) noexcept
            : data(std::move(input)), dataEndianType(endian)
    {
        if (size != 0)
        {
            dataPattern = system::allocMemoryForArray<uint16_t>(size, pattern, size * sizeof(uint16_t));
            if (dataPattern == nullptr) { Clear(); }
            fieldsCount = size;
        }
    }

    // Copy assignment constructor of BinaryStructuredDataEngine class.
    BinaryStructuredDataEngine::BinaryStructuredDataEngine (const BinaryStructuredDataEngine& other) noexcept
    {
        if (other.data == true)
        {
            data = other.data;
            if (data == true)
            {
                fieldsCount = other.fieldsCount;
                dataPattern = system::allocMemoryForArray<uint16_t>(fieldsCount, other.dataPattern.get(), fieldsCount * sizeof(uint16_t));
                dataEndianType = other.dataEndianType;
            }
        }
    }

    // Move assignment constructor of BinaryStructuredDataEngine class.
    BinaryStructuredDataEngine::BinaryStructuredDataEngine (BinaryStructuredDataEngine&& other) noexcept
    {
        if (other.data == true)
        {
            data = std::move(other.data);
            fieldsCount = other.fieldsCount;
            dataPattern = std::move(other.dataPattern);
            dataEndianType = other.dataEndianType;
            other.Clear();
        }
    }

    // Method that creates empty structured data template.
    bool BinaryStructuredDataEngine::CreateTemplate (const uint16_t* const pattern, const uint16_t size) noexcept
    {
        const std::size_t bytes = static_cast<std::size_t>(std::accumulate(pattern, pattern + size, 0));
        if (bytes == 0) { return false; }

        data = BinaryDataEngine(bytes, STRUCTURED_DATA_HANDLING_MODE, DATA_BIG_ENDIAN);
        if (data == false) { return false; }

        dataPattern = system::allocMemoryForArray<uint16_t>(size, pattern, size * sizeof(uint16_t));
        if (dataPattern == nullptr) {
            Clear();
            return false;
        }
        fieldsCount = size;
        return true;
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

            for (uint16_t jdx = 0; jdx < dataPattern[idx] / 2; ++jdx)
            {
                *data.GetAt(block + jdx) ^= *data.GetAt(block + dataPattern[idx] - jdx - 1);
                *data.GetAt(block + dataPattern[idx] - jdx - 1) ^= *data.GetAt(block + jdx);
                *data.GetAt(block + jdx) ^= *data.GetAt(block + dataPattern[idx] - jdx - 1);
            }
            block += dataPattern[idx];
        }
    }

    // Method that returns field value of structured data under selected index by reference.
    BinaryDataEngine BinaryStructuredDataEngine::GetFieldByReference (const uint16_t fieldIndex) const noexcept
    {
        if (fieldIndex < fieldsCount)
        {
            // Get index of first byte of selected field (Not consider the type of endian in which data are presented).
            const std::size_t byteIndex = static_cast<std::size_t>(std::accumulate(dataPattern.get(), dataPattern.get() + fieldIndex, 0));
            BinaryDataEngine result(data.GetAt(byteIndex), dataPattern[fieldIndex], dataEndianType);
            return result;
        }
        return BinaryDataEngine(DATA_MODE_DEFAULT, dataEndianType);
    }

    // Method that returns index of the first field in the selected bit-pattern where at least one bit is set.
    std::optional<uint16_t> BinaryStructuredDataEngine::GetNonemptyFieldIndex (const uint16_t start, const uint16_t* const pattern, const uint16_t size) const noexcept
    {
        if (start >= size) { return std::nullopt; }
        std::size_t offset = 0;  // Current bit offset.

        if (pattern != nullptr && size != 0)
        {
            const std::size_t bitCount = static_cast<std::size_t>(std::accumulate(pattern, pattern + size, 0));
            if (bitCount != data.BitsTransform().Length()) { return std::nullopt; }

            if (start != 0) {
                offset = static_cast<std::size_t>(std::accumulate(pattern, pattern + start, 0));
            }

            for (uint16_t field = start; field < size; ++field)
            {
                if (data.BitsTransform().Any(offset, offset + pattern[field] - 1) == true) {
                    return field;
                }
                offset += pattern[field];
            }
        }
        else if (dataPattern != nullptr)  // In this case the internal byte-pattern is used.
        {
            if (start != 0) {
                offset = static_cast<std::size_t>(std::accumulate(dataPattern.get(), dataPattern.get() + start, 0) * 8);
            }

            for (uint16_t field = start; field < fieldsCount; ++field)
            {
                if (data.BitsTransform().Any(offset, offset + dataPattern[field] * 8 - 1) == true) {
                    return field;
                }
                offset += dataPattern[field] * 8;
            }
        }
        return std::numeric_limits<uint16_t>::max();
    }

    // Method that returns the internal byte-pattern with length from the specified position.
    std::pair<uint16_t, const uint16_t*> BinaryStructuredDataEngine::GetPattern (const uint16_t fieldIndex) const noexcept
    {
        if (fieldIndex < fieldsCount) {
            return { fieldsCount - fieldIndex, &dataPattern[fieldIndex] };
        }
        return { 0, nullptr };
    }

    // Method that clears the internal binary structured data.
    void BinaryStructuredDataEngine::Clear(void) noexcept
    {
        data.Clear();
        fieldsCount = 0;
        dataPattern.reset(nullptr);
    }

    // Method that resets the internal state of BinaryStructuredDataEngine class to default state.
    void BinaryStructuredDataEngine::Reset(void) noexcept
    {
        data.Reset();
        fieldsCount = 0;
        dataPattern.reset(nullptr);
        dataEndianType = BinaryDataEngine::system_endian;
    }

    // Method that outputs internal binary structured data by pattern in string format.
    std::string BinaryStructuredDataEngine::ToFormattedString (void) const noexcept
    {
        std::ostringstream result;
        if (data.IsEmpty() == false)
        {
            result.unsetf(std::ios_base::boolalpha);
            uint16_t patternBlock = 0, blockBitCount = 0;

            result << "\nField 1:   ";

            if (dataEndianType == DATA_BIG_ENDIAN)
            {
                for (std::size_t idx = 0; idx < data.BitsTransform().Length(); ++idx)
                {
                    if (idx % 8 == 0 && blockBitCount != 0) { result << ' '; }
                    result << data.BitsTransform().GetBitValue(idx);
                    if (++blockBitCount == dataPattern[patternBlock] * 8) {
                        if (++patternBlock == fieldsCount) { break; }
                        result << "\nField " << patternBlock + 1 << ":   ";
                        blockBitCount = 0;
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
                            if (commonBitCount++ % 8 == 0 && blockBitCount != 0) { result << ' '; }
                            result << data.BitsTransform().GetBitValue(idx);
                            if (++blockBitCount == dataPattern[patternBlock] * 8) {
                                if (++patternBlock == fieldsCount) { break; }
                                result << "\nField " << patternBlock + 1 << ":   ";
                                blockBitCount = 0;
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
        if (this != &other && other.data == true)
        {
            data = other.data;
            if (data == true)
            {
                fieldsCount = other.fieldsCount;
                dataPattern = system::allocMemoryForArray<uint16_t>(fieldsCount, other.dataPattern.get(), fieldsCount * sizeof(uint16_t));
                dataEndianType = other.dataEndianType;
            }
        }
        return *this;
    }

    // Move assignment operator of BinaryStructuredDataEngine class.
    BinaryStructuredDataEngine& BinaryStructuredDataEngine::operator= (BinaryStructuredDataEngine&& other) noexcept
    {
        if (this != &other && other.data == true)
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
