// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#include <sstream>  // std::ostringstream.

#include "../../include/framework/BinaryDataEngine.hpp"


namespace analyzer::framework::common::types
{
    // Method that returns the correct position of selected bit in stored data in any data endian.
    std::pair<std::size_t, std::byte> BinaryDataEngine::BitStreamEngine::GetBitPosition (const std::size_t index) const noexcept
    {
        if ((storedData.dataModeType & DATA_MODE_DEPENDENT) != 0U)
        {
            if (storedData.dataEndianType == DATA_LITTLE_ENDIAN) {
                return { index >> 3, std::byte(0x01) << (index % 8) };
            }
            // If data endian type is DATA_BIG_ENDIAN.
            return { storedData.length - (index >> 3) - 1, std::byte(0x01) << (index % 8) };
        }
        // If data handling mode type is DATA_MODE_INDEPENDENT.
        return { index >> 3, std::byte(0x80) >> (index % 8) };
    }

    // Method that returns bit value under the specified index.
    bool BinaryDataEngine::BitStreamEngine::GetBitValue (const std::size_t index) const noexcept
    {
        const auto [part, shift] = GetBitPosition(index);
        return ((storedData.data[part] & shift) != std::byte(0x00));
    }

    // Method that performs direct left bit shift by a specified bit offset.
    const BinaryDataEngine::BitStreamEngine& BinaryDataEngine::BitStreamEngine::ShiftLeft (const std::size_t shift, const  bool fillBit) const noexcept
    {
        if (storedData == true && shift != 0)
        {
            const std::byte fillByte = (fillBit == false ? std::byte(0x00) : std::byte(0xFF));
            if (shift >= Length()) {
                memset(storedData.data.get(), static_cast<int32_t>(fillByte), storedData.length);
                return *this;
            }

            const std::size_t countOfBytesShift = (shift >> 3);
            if (countOfBytesShift > 0) {
                storedData.BytesTransform().ShiftLeft(countOfBytesShift, fillByte);
            }

            const std::size_t tailBits = shift % 8;
            if (tailBits > 0)
            {
                if ((storedData.dataModeType & DATA_MODE_DEPENDENT) != 0U && storedData.dataEndianType == DATA_LITTLE_ENDIAN)
                {
                    for (std::size_t idx = storedData.length - 1; idx != 0; --idx) {
                        storedData.data[idx] = (storedData.data[idx - 1] >> (8 - tailBits)) | (storedData.data[idx] << tailBits);
                    }
                    storedData.data[0] <<= tailBits;
                    if (fillBit == true) {
                        storedData.data[0] |= (std::byte(0xFF) >> (8 - tailBits));
                    }
                }
                else  // If data endian type is DATA_BIG_ENDIAN or if data handling mode type is DATA_MODE_INDEPENDENT.
                {
                    const std::size_t lastIndex = storedData.length - 1;
                    for (std::size_t idx = 0; idx < lastIndex; ++idx) {
                        storedData.data[idx] = (storedData.data[idx + 1] >> (8 - tailBits)) | (storedData.data[idx] << tailBits);
                    }
                    storedData.data[lastIndex] <<= tailBits;
                    if (fillBit == true) {
                        storedData.data[lastIndex] |= (std::byte(0xFF) >> (8 - tailBits));
                    }
                }
            }
        }
        return *this;
    }

    // Method that performs direct right bit shift by a specified bit offset.
    const BinaryDataEngine::BitStreamEngine& BinaryDataEngine::BitStreamEngine::ShiftRight (const std::size_t shift, const bool fillBit) const noexcept
    {
        if (storedData == true && shift != 0)
        {
            const std::byte fillByte = (fillBit == false ? std::byte(0x00) : std::byte(0xFF));
            if (shift >= Length()) {
                memset(storedData.data.get(), static_cast<int32_t>(fillByte), storedData.length);
                return *this;
            }

            const std::size_t countOfBytesShift = (shift >> 3);
            if (countOfBytesShift > 0) {
                storedData.BytesTransform().ShiftRight(countOfBytesShift, fillByte);
            }

            const std::size_t tailBits = shift % 8;
            if (tailBits > 0)
            {
                if ((storedData.dataModeType & DATA_MODE_DEPENDENT) != 0U && storedData.dataEndianType == DATA_LITTLE_ENDIAN)
                {
                    const std::size_t lastIndex = storedData.length - 1;
                    for (std::size_t idx = 0; idx < lastIndex; ++idx) {
                        storedData.data[idx] = (storedData.data[idx + 1] << (8 - tailBits)) | (storedData.data[idx] >> tailBits);
                    }
                    storedData.data[lastIndex] >>= tailBits;
                    if (fillBit == true) {
                        storedData.data[lastIndex] |= (std::byte(0xFF) << (8 - tailBits));
                    }
                }
                else  // If data endian type is DATA_BIG_ENDIAN or if data handling mode type is DATA_MODE_INDEPENDENT.
                {
                    for (std::size_t idx = storedData.length - 1; idx != 0; --idx) {
                        storedData.data[idx] = (storedData.data[idx - 1] << (8 - tailBits)) | (storedData.data[idx] >> tailBits);
                    }
                    storedData.data[0] >>= tailBits;
                    if (fillBit == true) {
                        storedData.data[0] |= (std::byte(0xFF) << (8 - tailBits));
                    }
                }
            }
        }
        return *this;
    }

    // Method that performs round left bit shift by a specified bit offset.
    const BinaryDataEngine::BitStreamEngine& BinaryDataEngine::BitStreamEngine::RoundShiftLeft (std::size_t shift) const noexcept
    {
        if (storedData == true && shift != 0)
        {
            if (shift >= Length()) {
                shift %= Length();
            }

            const std::size_t countOfBytesShift = (shift >> 3);
            if (countOfBytesShift > 0) {
                storedData.BytesTransform().RoundShiftLeft(countOfBytesShift);
            }

            const std::size_t tailBits = shift % 8;
            if (tailBits > 0)
            {
                if ((storedData.dataModeType & DATA_MODE_DEPENDENT) != 0U && storedData.dataEndianType == DATA_LITTLE_ENDIAN)
                {
                    const std::byte last = storedData.data[storedData.length - 1];
                    for (std::size_t idx = storedData.length - 1; idx != 0; --idx) {
                        storedData.data[idx] = (storedData.data[idx - 1] >> (8 - tailBits)) | (storedData.data[idx] << tailBits);
                    }
                    storedData.data[0] = (last >> (8 - tailBits) | (storedData.data[0] << tailBits));
                }
                else  // If data endian type is DATA_BIG_ENDIAN or if data handling mode type is DATA_MODE_INDEPENDENT.
                {
                    const std::byte first = storedData.data[0];
                    const std::size_t lastIndex = storedData.length - 1;
                    for (std::size_t idx = 0; idx < lastIndex; ++idx) {
                        storedData.data[idx] = (storedData.data[idx + 1] >> (8 - tailBits)) | (storedData.data[idx] << tailBits);
                    }
                    storedData.data[lastIndex] = (first >> (8 - tailBits) | storedData.data[lastIndex] << tailBits);
                }
            }
        }
        return *this;
    }

    // Method that performs round right bit shift by a specified bit offset.
    const BinaryDataEngine::BitStreamEngine& BinaryDataEngine::BitStreamEngine::RoundShiftRight (std::size_t shift) const noexcept
    {
        if (storedData == true && shift != 0)
        {
            if (shift >= Length()) {
                shift %= Length();
            }

            const std::size_t countOfBytesShift = (shift >> 3);
            if (countOfBytesShift > 0) {
                storedData.BytesTransform().RoundShiftRight(countOfBytesShift);
            }

            const std::size_t tailBits = shift % 8;
            if (tailBits > 0)
            {
                if ((storedData.dataModeType & DATA_MODE_DEPENDENT) != 0U && storedData.dataEndianType == DATA_LITTLE_ENDIAN)
                {
                    const std::size_t lastIndex = storedData.length - 1;
                    const std::byte first = storedData.data[lastIndex];
                    for (std::size_t idx = 0; idx < lastIndex; ++idx) {
                        storedData.data[idx] = (storedData.data[idx + 1] << (8 - tailBits)) | (storedData.data[idx] >> tailBits);
                    }
                    storedData.data[lastIndex] = (first << (8 - tailBits) | storedData.data[lastIndex] >> tailBits);
                }
                else  // If data endian type is DATA_BIG_ENDIAN or if data handling mode type is DATA_MODE_INDEPENDENT.
                {
                    const std::byte last = storedData.data[storedData.length - 1];
                    for (std::size_t idx = storedData.length - 1; idx != 0; --idx) {
                        storedData.data[idx] = (storedData.data[idx - 1] << (8 - tailBits)) | (storedData.data[idx] >> tailBits);
                    }
                    storedData.data[0] = (last << (8 - tailBits) | (storedData.data[0] >> tailBits));
                }
            }
        }
        return *this;
    }

    // Method that checks the bit under the specified index.
    bool BinaryDataEngine::BitStreamEngine::Test (const std::size_t index) const noexcept
    {
        return (index < Length() && GetBitValue(index));
    }

    // Method that returns bit sequence characteristic when all bit are set in block of stored data.
    bool BinaryDataEngine::BitStreamEngine::All (std::size_t first, std::size_t last) const noexcept
    {
        if (last == npos) { last = Length() - 1; }
        if (first > last || last >= Length()) { return false; }

        const std::size_t size = last - first + 1;
        // If first index starts at the beginning of the byte and test sequence length more then eight bits.
        if (first % 8 == 0 && size >= 8)
        {
            for (std::size_t idx = 0; idx < size / 8; ++idx)
            {
                if (storedData.data[GetBitPosition(first).first] != std::byte(0xFF)) {
                    return false;
                }
                first += 8;
            }
        }

        // Rest part of bit sequence or this code block is used if the byte-optimization did not work.
        while (first <= last)
        {
            if (GetBitValue(first++) == false) {
                return false;
            }
        }
        return true;
    }

    // Method that returns bit sequence characteristic when any of the bits are set in block of stored data.
    bool BinaryDataEngine::BitStreamEngine::Any (std::size_t first, std::size_t last) const noexcept
    {
        if (last == npos) { last = Length() - 1; }
        if (first > last || last >= Length()) { return false; }

        const std::size_t size = last - first + 1;
        // If first index starts at the beginning of the byte and test sequence length more then eight bits.
        if (first % 8 == 0 && size >= 8)
        {
            for (std::size_t idx = 0; idx < size / 8; ++idx)
            {
                if (storedData.data[GetBitPosition(first).first] != std::byte(0x00)) {
                    return true;
                }
                first += 8;
            }
        }

        // Rest part of bit sequence or this code block is used if the byte-optimization did not work.
        while (first <= last)
        {
            if (GetBitValue(first++) == true) {
                return true;
            }
        }
        return false;
    }

    // Method that returns bit sequence characteristic when none of the bits are set in block of stored data.
    bool BinaryDataEngine::BitStreamEngine::None (std::size_t first, std::size_t last) const noexcept
    {
        if (last == npos) { last = Length() - 1; }
        if (first > last || last >= Length()) { return false; }

        const std::size_t size = last - first + 1;
        // If first index starts at the beginning of the byte and test sequence length more then eight bits.
        if (first % 8 == 0 && size >= 8)
        {
            for (std::size_t idx = 0; idx < size / 8; ++idx)
            {
                if (storedData.data[GetBitPosition(first).first] != std::byte(0x00)) {
                    return false;
                }
                first += 8;
            }
        }

        // Rest part of bit sequence or this code block is used if the byte-optimization did not work.
        while (first <= last)
        {
            if (GetBitValue(first++) == true) {
                return false;
            }
        }
        return true;
    }

    // Method that returns the number of bits that are set in the selected interval of stored data.
    std::size_t BinaryDataEngine::BitStreamEngine::Count (std::size_t first, std::size_t last) const noexcept
    {
        if (last == npos) { last = Length() - 1; }
        if (first > last || last >= Length()) { return npos; }

        std::size_t count = 0;
        while (first <= last) {
            count += (GetBitValue(first++) == true) ? 1 : 0;
        }
        return count;
    }

    // Method that returns position of the first set bit in the selected interval of stored data.
    std::optional<std::size_t> BinaryDataEngine::BitStreamEngine::GetFirstIndex (const std::size_t first, std::size_t last, const bool isRelative) const noexcept
    {
        if (last == npos) { last = Length() - 1; }
        if (first > last || last >= Length()) { return std::nullopt; }

        std::size_t index = first;
        while (index <= last)
        {
            if (GetBitValue(index) == true) {
                return ((isRelative == true) ? index - first : index);
            }
            index++;
        }
        return npos;
    }

    // Method that returns position of the last set bit in the selected interval of stored data.
    std::optional<std::size_t> BinaryDataEngine::BitStreamEngine::GetLastIndex (const std::size_t first, std::size_t last, const bool isRelative) const noexcept
    {
        if (last == npos) { last = Length() - 1; }
        if (first > last || last >= Length()) { return std::nullopt; }

        std::size_t index = last;
        while (index >= first)
        {
            if (GetBitValue(index) == true) {
                return ((isRelative == true) ? index - first : index);
            }
            index--;
        }
        return npos;
    }

    // Method that sets the bit under the specified index to new value.
    const BinaryDataEngine::BitStreamEngine& BinaryDataEngine::BitStreamEngine::Set (const std::size_t index, const bool fillBit) const noexcept
    {
        if (index >= Length()) { return *this; }

        const auto [part, shift] = GetBitPosition(index);
        if (fillBit == true) {
            storedData.data[part] |= shift;
        }
        else { storedData.data[part] &= ~shift; }
        return *this;
    }

    // Method that reverses a sequence of bits under the specified first/last indexes.
    const BinaryDataEngine::BitStreamEngine& BinaryDataEngine::BitStreamEngine::Reverse (std::size_t first, std::size_t last) const noexcept
    {
        if (last == npos) { last = Length() - 1; }
        if (first > last || last >= Length()) { return *this; }

        while (first < last)
        {
            const auto [partFirst, shiftFirst] = GetBitPosition(first);
            const auto [partLast, shiftLast] = GetBitPosition(last);

            if (((storedData.data[partFirst] & shiftFirst) != std::byte(0x00)) !=
                ((storedData.data[partLast] & shiftLast) != std::byte(0x00)))
            {
                storedData.data[partFirst] ^= shiftFirst;
                storedData.data[partLast] ^= shiftLast;
            }
            first++;
            last--;
        }
        return *this;
    }

    // Method that inverts the bit under the specified index.
    const BinaryDataEngine::BitStreamEngine& BinaryDataEngine::BitStreamEngine::Invert (const std::size_t index) const noexcept
    {
        if (index >= Length()) { return *this; }

        const auto [part, shift] = GetBitPosition(index);
        storedData.data[part] ^= shift;
        return *this;
    }

    // Method that inverts the range of bits under the specified first/last indexes.
    const BinaryDataEngine::BitStreamEngine& BinaryDataEngine::BitStreamEngine::InvertBlock (std::size_t first, std::size_t last) const noexcept
    {
        if (last == npos) { last = Length() - 1; }
        if (first > last || last >= Length()) { return *this; }

        while (first <= last) {
            Invert(first++);
        }
        return *this;
    }

    // Method that outputs internal binary data in string format.
    std::string BinaryDataEngine::BitStreamEngine::ToString (std::size_t first, std::size_t last) const noexcept
    {
        if (last == npos) { last = Length() - 1; }
        if (first > last || last >= Length()) { return std::string(); }

        std::ostringstream result;
        if (storedData.IsEmpty() == false)
        {
            result.unsetf(std::ios_base::boolalpha);
            if ((storedData.dataModeType & DATA_MODE_DEPENDENT) != 0U)
            {
                while (last >= first && last != 0)
                {
                    if (last % 8 == 0) { result << ' '; }
                    result << GetBitValue(last--);
                }
                if (last == 0) { result << GetBitValue(0); }
            }
            else  // If data handling mode type is DATA_MODE_INDEPENDENT.
            {
                while (first <= last)
                {
                    result << GetBitValue(first++);
                    if (first % 8 == 0) { result << ' '; }
                }
            }
        }
        return result.str();
    }

    // Bitwise left shift assignment operator that performs direct left bit shift by a specified bit offset.
    const BinaryDataEngine::BitStreamEngine& BinaryDataEngine::BitStreamEngine::operator<<= (const std::size_t shift) const noexcept
    {
        return ShiftLeft(shift, false);
    }

    // Bitwise right shift assignment operator that performs direct right bit shift by a specified bit offset.
    const BinaryDataEngine::BitStreamEngine& BinaryDataEngine::BitStreamEngine::operator>>= (const std::size_t shift) const noexcept
    {
        return ShiftRight(shift, false);
    }

    // Logical assignment bitwise AND operator that transforms internal binary data.
    const BinaryDataEngine::BitStreamEngine& BinaryDataEngine::BitStreamEngine::operator&= (const BinaryDataEngine::BitStreamEngine& other) const noexcept
    {
        if (storedData == true)
        {
            const auto& currentByteEngine = storedData.BytesTransform();
            const auto& otherByteEngine = other.storedData.BytesTransform();

            // If left operand (current) has data with longer or equal length.
            if (storedData.length >= other.storedData.length)
            {
                for (std::size_t idx = 0; idx < otherByteEngine.Length(); ++idx) {
                    (*currentByteEngine.GetAt(idx)) &= otherByteEngine[idx].value();
                }
                for (std::size_t idx = otherByteEngine.Length(); idx < currentByteEngine.Length(); ++idx) {
                    (*currentByteEngine.GetAt(idx)) &= std::byte(0x00);
                }
            }
            // If right operand (other) has data with longer length but data handling mode DATA_MODE_SAFE_OPERATOR is set.
            else if ((storedData.dataModeType & DATA_MODE_SAFE_OPERATOR) != 0U)
            {
                for (std::size_t idx = 0; idx < currentByteEngine.Length(); ++idx) {
                    (*currentByteEngine.GetAt(idx)) &= otherByteEngine[idx].value();
                }
            }
            else  // If right operand (other) has data with longer length and data handling mode is DATA_MODE_UNSAFE_OPERATOR.
            {
                if (storedData.dataEndianType == DATA_LITTLE_ENDIAN)
                {
                    auto newData = system::allocMemoryForArray<std::byte>(other.storedData.length, storedData.data.get(), storedData.length);
                    if (newData != nullptr)
                    {
                        storedData.data = std::move(newData);
                        storedData.length = other.storedData.length;
                        for (std::size_t idx = 0; idx < currentByteEngine.Length(); ++idx) {
                            (*currentByteEngine.GetAt(idx)) &= otherByteEngine[idx].value();
                        }
                    }
                }
                else  // If data endian type is DATA_BIG_ENDIAN.
                {
                    auto newData = system::allocMemoryForArray<std::byte>(other.storedData.length);
                    if (newData != nullptr)
                    {
                        const std::size_t diff = other.storedData.length - storedData.length;
                        memset(newData.get(), 0, diff);
                        memcpy(newData.get() + diff, storedData.data.get(), storedData.length);
                        storedData.data = std::move(newData);
                        storedData.length = other.storedData.length;
                        for (std::size_t idx = 0; idx < currentByteEngine.Length(); ++idx) {
                            (*currentByteEngine.GetAt(idx)) &= otherByteEngine[idx].value();
                        }
                    }
                }
            }
        }
        return *this;
    }

    // Logical assignment bitwise OR operator that transforms internal binary data.
    const BinaryDataEngine::BitStreamEngine& BinaryDataEngine::BitStreamEngine::operator|= (const BinaryDataEngine::BitStreamEngine& other) const noexcept
    {
        if (storedData == true)
        {
            const auto& currentByteEngine = storedData.BytesTransform();
            const auto& otherByteEngine = other.storedData.BytesTransform();

            // If left operand (current) has data with longer or equal length.
            if (storedData.length >= other.storedData.length)
            {
                for (std::size_t idx = 0; idx < otherByteEngine.Length(); ++idx) {
                    (*currentByteEngine.GetAt(idx)) |= otherByteEngine[idx].value();
                }
            }
            // If right operand (other) has data with longer length but data handling mode DATA_MODE_SAFE_OPERATOR is set.
            else if ((storedData.dataModeType & DATA_MODE_SAFE_OPERATOR) != 0U)
            {
                for (std::size_t idx = 0; idx < currentByteEngine.Length(); ++idx) {
                    (*currentByteEngine.GetAt(idx)) |= otherByteEngine[idx].value();
                }
            }
            else  // If right operand (other) has data with longer length and data handling mode is DATA_MODE_UNSAFE_OPERATOR.
            {
                if (storedData.dataEndianType == DATA_LITTLE_ENDIAN)
                {
                    auto newData = system::allocMemoryForArray<std::byte>(other.storedData.length, storedData.data.get(), storedData.length);
                    if (newData != nullptr)
                    {
                        storedData.data = std::move(newData);
                        storedData.length = other.storedData.length;
                        for (std::size_t idx = 0; idx < currentByteEngine.Length(); ++idx) {
                            (*currentByteEngine.GetAt(idx)) |= otherByteEngine[idx].value();
                        }
                    }
                }
                else  // If data endian type is DATA_BIG_ENDIAN.
                {
                    auto newData = system::allocMemoryForArray<std::byte>(other.storedData.length);
                    if (newData != nullptr)
                    {
                        const std::size_t diff = other.storedData.length - storedData.length;
                        memset(newData.get(), 0, diff);
                        memcpy(newData.get() + diff, storedData.data.get(), storedData.length);
                        storedData.data = std::move(newData);
                        storedData.length = other.storedData.length;
                        for (std::size_t idx = 0; idx < currentByteEngine.Length(); ++idx) {
                            (*currentByteEngine.GetAt(idx)) |= otherByteEngine[idx].value();
                        }
                    }
                }
            }
        }
        return *this;
    }

    // Logical assignment bitwise XOR operator that transforms internal binary data.
    const BinaryDataEngine::BitStreamEngine& BinaryDataEngine::BitStreamEngine::operator^= (const BinaryDataEngine::BitStreamEngine& other) const noexcept
    {
        if (storedData == true)
        {
            const auto& currentByteEngine = storedData.BytesTransform();
            const auto& otherByteEngine = other.storedData.BytesTransform();

            // If left operand (current) has data with longer or equal length.
            if (storedData.length >= other.storedData.length)
            {
                for (std::size_t idx = 0; idx < otherByteEngine.Length(); ++idx) {
                    (*currentByteEngine.GetAt(idx)) ^= otherByteEngine[idx].value();
                }
            }
            // If right operand (other) has data with longer length but data handling mode DATA_MODE_SAFE_OPERATOR is set.
            else if ((storedData.dataModeType & DATA_MODE_SAFE_OPERATOR) != 0U)
            {
                for (std::size_t idx = 0; idx < currentByteEngine.Length(); ++idx) {
                    (*currentByteEngine.GetAt(idx)) ^= otherByteEngine[idx].value();
                }
            }
            else  // If right operand (other) has data with longer length and data handling mode is DATA_MODE_UNSAFE_OPERATOR.
            {
                if (storedData.dataEndianType == DATA_LITTLE_ENDIAN)
                {
                    auto newData = system::allocMemoryForArray<std::byte>(other.storedData.length, storedData.data.get(), storedData.length);
                    if (newData != nullptr)
                    {
                        storedData.data = std::move(newData);
                        storedData.length = other.storedData.length;
                        for (std::size_t idx = 0; idx < currentByteEngine.Length(); ++idx) {
                            (*currentByteEngine.GetAt(idx)) ^= otherByteEngine[idx].value();
                        }
                    }
                }
                else  // If data endian type is DATA_BIG_ENDIAN.
                {
                    auto newData = system::allocMemoryForArray<std::byte>(other.storedData.length);
                    if (newData != nullptr)
                    {
                        const std::size_t diff = other.storedData.length - storedData.length;
                        memset(newData.get(), 0, diff);
                        memcpy(newData.get() + diff, storedData.data.get(), storedData.length);
                        storedData.data = std::move(newData);
                        storedData.length = other.storedData.length;
                        for (std::size_t idx = 0; idx < currentByteEngine.Length(); ++idx) {
                            (*currentByteEngine.GetAt(idx)) ^= otherByteEngine[idx].value();
                        }
                    }
                }
            }
        }
        return *this;
    }

}  // namespace types.
