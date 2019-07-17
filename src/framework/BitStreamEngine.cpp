// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#include <sstream>  // std::ostringstream.
#include <netinet/in.h>  // htonl.

#include "../../include/framework/BinaryDataEngine.hpp"


namespace analyzer::framework::common::types
{
    // Method that returns the correct position of byte which store the selected bit under specified index in stored binary data in any data endian.
    std::size_t BinaryDataEngine::BitStreamInformationEngine::GetByteIndex (const std::size_t index) const noexcept
    {
        if ((storedData->dataModeType & DATA_MODE_DEPENDENT) != 0U)
        {
            if (storedData->dataEndianType == DATA_LITTLE_ENDIAN) {
                return index >> 3;
            }
            if (storedData->dataEndianType == DATA_BIG_ENDIAN) {
                return storedData->length - (index >> 3) - 1;
            }
            // If data endian type is DATA_REVERSE_BIG_ENDIAN.
            return storedData->length - (index >> 3) - 1;
        }
        // If data handling mode type is DATA_MODE_INDEPENDENT.
        return index >> 3;
    }

    // Method that returns the correct position of selected bit in stored data in any data endian.
    std::pair<std::size_t, std::byte> BinaryDataEngine::BitStreamInformationEngine::GetBitPosition (const std::size_t index) const noexcept
    {
        if ((storedData->dataModeType & DATA_MODE_DEPENDENT) != 0U)
        {
            if (storedData->dataEndianType == DATA_LITTLE_ENDIAN) {
                return { index >> 3, LowBitInByte << (index % 8) };
            }
            if (storedData->dataEndianType == DATA_BIG_ENDIAN) {
                return { storedData->length - (index >> 3) - 1, LowBitInByte << (index % 8) };
            }
            // If data endian type is DATA_REVERSE_BIG_ENDIAN.
            return { storedData->length - (index >> 3) - 1, HighBitInByte >> (index % 8) };
        }
        // If data handling mode type is DATA_MODE_INDEPENDENT.
        return { index >> 3, HighBitInByte >> (index % 8) };
    }

    // Method that returns bit value under the specified index.
    bool BinaryDataEngine::BitStreamInformationEngine::GetBitValue (const std::size_t index) const noexcept
    {
        const auto [part, shift] = GetBitPosition(index);
        return ((storedData->data[part] & shift) != LowByte);
    }

    // Method that checks the bit under the specified index.
    bool BinaryDataEngine::BitStreamInformationEngine::Test (const std::size_t index) const noexcept
    {
        return (index < Length() && GetBitValue(index));
    }

    // Method that returns bit sequence characteristic when all bit are set in block of stored data.
    bool BinaryDataEngine::BitStreamInformationEngine::All (std::size_t first, std::size_t last) const noexcept
    {
        if (last == NPOS) { last = Length() - 1; }
        if (first > last || last >= Length()) { return false; }

        const std::size_t size = last - first + 1;
        // If first index starts at the beginning of the byte and test sequence length more then eight bits.
        if (first % 8 == 0 && size >= 8)
        {
            for (std::size_t idx = 0; idx < size / 8; ++idx)
            {
                if (storedData->data[GetBitPosition(first).first] != HighByte) {
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
    bool BinaryDataEngine::BitStreamInformationEngine::Any (std::size_t first, std::size_t last) const noexcept
    {
        if (last == NPOS) { last = Length() - 1; }
        if (first > last || last >= Length()) { return false; }

        const std::size_t bits = last - first + 1;
        // Bit sequence size less then 32-bit value.
        if (bits <= 24 || (first % 8 == 0 && bits <= 32))
        {
            std::size_t startByteIndex = 0, endByteIndex = 0;
            if (storedData->dataEndianType == DATA_LITTLE_ENDIAN || storedData->IsDependentDataMode() == false)
            {
                startByteIndex = GetByteIndex(first);
                endByteIndex = GetByteIndex(last);
            }
            else  // DATA_BIG_ENDIAN or DATA_REVERSE_BIG_ENDIAN or DATA_DEPENDENT_MODE.
            {
                startByteIndex = GetByteIndex(last);
                endByteIndex = GetByteIndex(first);
            }
            const std::size_t bytes = endByteIndex - startByteIndex + 1;

            uint32_t value = 0;
            memcpy(&value, &storedData->data[startByteIndex], bytes);

            // Data mode is DATA_MODE_INDEPENDENT.
            if (storedData->IsDependentDataMode() == false)
            {
                // Local endian is DATA_LITTLE_ENDIAN.
                if (BinaryDataEngine::system_endian == DATA_LITTLE_ENDIAN) {
                    value = htonl(value);
                }
                value <<= first % 8;
                value >>= 32 - bits;
            }
            else  // Data mode is DATA_MODE_DEPENDENT.
            {
                if (storedData->dataEndianType == DATA_LITTLE_ENDIAN)
                {
                    if (BinaryDataEngine::system_endian == DATA_BIG_ENDIAN) {
                        value = htonl(value);
                    }
                    value >>= first % 8;
                    value <<= 32 - bits;
                }
                else if (storedData->dataEndianType == DATA_BIG_ENDIAN)
                {
                    if (BinaryDataEngine::system_endian == DATA_LITTLE_ENDIAN) {
                        value = htonl(value);
                    }
                    value <<= 8 - (last % 8) - 1;
                    value >>= 32 - bits;
                }
                else  // Data endian is DATA_REVERSE_BIG_ENDIAN.
                {
                    if (BinaryDataEngine::system_endian == DATA_BIG_ENDIAN) {
                        value = htonl(value);
                    }
                    value >>= 8 - (last % 8);
                    value <<= 32 - bits;
                }
            }
            return value != 0;
        }

        // First index starts at the beginning of the byte and test sequence length more then eight bits.
        if (first % 8 == 0 && bits >= 8)
        {
            std::size_t startIndex = 0;
            if (storedData->dataEndianType == DATA_LITTLE_ENDIAN || storedData->IsDependentDataMode() == false) {
                startIndex = GetByteIndex(first);
            }
            else {
                startIndex = GetByteIndex(first + (bits / 8) * 8 - 1);
            }

            for (std::size_t idx = 0; idx < bits / 8; ++idx)
            {
                if (storedData->data[startIndex + idx] != LowByte) {
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
    bool BinaryDataEngine::BitStreamInformationEngine::None (std::size_t first, std::size_t last) const noexcept
    {
        if (last == NPOS) { last = Length() - 1; }
        if (first > last || last >= Length()) { return false; }

        const std::size_t size = last - first + 1;
        // If first index starts at the beginning of the byte and test sequence length more then eight bits.
        if (first % 8 == 0 && size >= 8)
        {
            for (std::size_t idx = 0; idx < size / 8; ++idx)
            {
                if (storedData->data[GetByteIndex(first)] != LowByte) {
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
    std::size_t BinaryDataEngine::BitStreamInformationEngine::Count (std::size_t first, std::size_t last) const noexcept
    {
        if (last == NPOS) { last = Length() - 1; }
        if (first > last || last >= Length()) { return NPOS; }

        std::size_t count = 0;
        while (first <= last) {
            count += (GetBitValue(first++) == true) ? 1 : 0;
        }
        return count;
    }

    // Method that returns position of the first set bit in the selected interval of stored data.
    std::optional<std::size_t> BinaryDataEngine::BitStreamInformationEngine::GetFirstIndex (const std::size_t first, std::size_t last, const bool value, const bool isRelative) const noexcept
    {
        if (last == NPOS) { last = Length() - 1; }
        if (first > last || last >= Length()) { return std::nullopt; }

        std::size_t index = first;
        while (index <= last)
        {
            if (GetBitValue(index) == value) {
                return ((isRelative == true) ? index - first : index);
            }
            index++;
        }
        return NPOS;
    }

    // Method that returns position of the last set bit in the selected interval of stored data.
    std::optional<std::size_t> BinaryDataEngine::BitStreamInformationEngine::GetLastIndex (const std::size_t first, std::size_t last, const bool value, const bool isRelative) const noexcept
    {
        if (last == NPOS) { last = Length() - 1; }
        if (first > last || last >= Length()) { return std::nullopt; }

        int64_t index = last;
        while (index >= static_cast<int64_t>(first))
        {
            if (GetBitValue(index) == value) {
                return ((isRelative == true) ? index - first : index);
            }
            index--;
        }
        return NPOS;
    }

    // Method that outputs internal binary data in string format.
    std::string BinaryDataEngine::BitStreamInformationEngine::ToString (std::size_t first, std::size_t last) const noexcept
    {
        if (last == NPOS) { last = Length() - 1; }
        if (first > last || last >= Length()) { return std::string(); }

        std::ostringstream result;
        if (storedData->IsEmpty() == false)
        {
            result.unsetf(std::ios_base::boolalpha);
            if ((storedData->dataModeType & DATA_MODE_DEPENDENT) != 0U)
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




    // Method that performs direct left bit shift by a specified bit offset.
    BinaryDataEngine::BitStreamTransformEngine& BinaryDataEngine::BitStreamTransformEngine::ShiftLeft (const std::size_t shift, const bool fillBit) noexcept
    {
        if (*storedData == true && shift != 0)
        {
            const std::byte fillByte = (fillBit == false ? LowByte : HighByte);
            if (shift >= Length()) {
                memset(storedData->data.get(), static_cast<int32_t>(fillByte), storedData->length);
                return *this;
            }

            const std::size_t countOfBytesShift = (shift >> 3);
            if (countOfBytesShift > 0) {
                storedData->BytesTransform().ShiftLeft(countOfBytesShift, fillByte);
            }

            const std::size_t tailBits = shift % 8;
            if (tailBits > 0)
            {
                if ((storedData->dataModeType & DATA_MODE_DEPENDENT) != 0U)
                {
                    if (storedData->dataEndianType == DATA_LITTLE_ENDIAN)
                    {
                        for (std::size_t idx = storedData->length - 1; idx != 0; --idx) {
                            storedData->data[idx] = (storedData->data[idx - 1] >> (8 - tailBits)) | (storedData->data[idx] << tailBits);
                        }
                        storedData->data[0] <<= tailBits;
                        if (fillBit == true) {
                            storedData->data[0] |= (HighByte >> (8 - tailBits));
                        }
                    }
                    else if (storedData->dataEndianType == DATA_REVERSE_BIG_ENDIAN)
                    {
                        const std::size_t lastIndex = storedData->length - 1;
                        for (std::size_t idx = 0; idx < lastIndex; ++idx) {
                            storedData->data[idx] = (storedData->data[idx + 1] << (8 - tailBits)) | (storedData->data[idx] >> tailBits);
                        }
                        storedData->data[lastIndex] >>= tailBits;
                        if (fillBit == true) {
                            storedData->data[lastIndex] |= (HighByte << (8 - tailBits));
                        }
                    }
                }
                else  // If data endian type is DATA_BIG_ENDIAN or if data handling mode type is DATA_MODE_INDEPENDENT.
                {
                    const std::size_t lastIndex = storedData->length - 1;
                    for (std::size_t idx = 0; idx < lastIndex; ++idx) {
                        storedData->data[idx] = (storedData->data[idx + 1] >> (8 - tailBits)) | (storedData->data[idx] << tailBits);
                    }
                    storedData->data[lastIndex] <<= tailBits;
                    if (fillBit == true) {
                        storedData->data[lastIndex] |= (HighByte >> (8 - tailBits));
                    }
                }
            }
        }
        return *this;
    }

    // Method that performs direct right bit shift by a specified bit offset.
    BinaryDataEngine::BitStreamTransformEngine& BinaryDataEngine::BitStreamTransformEngine::ShiftRight (const std::size_t shift, const bool fillBit) noexcept
    {
        if (*storedData == true && shift != 0)
        {
            const std::byte fillByte = (fillBit == false ? LowByte : HighByte);
            if (shift >= Length()) {
                memset(storedData->data.get(), static_cast<int32_t>(fillByte), storedData->length);
                return *this;
            }

            const std::size_t countOfBytesShift = (shift >> 3);
            if (countOfBytesShift > 0) {
                storedData->BytesTransform().ShiftRight(countOfBytesShift, fillByte);
            }

            const std::size_t tailBits = shift % 8;
            if (tailBits > 0)
            {
                if ((storedData->dataModeType & DATA_MODE_DEPENDENT) != 0U && storedData->dataEndianType == DATA_LITTLE_ENDIAN)
                {
                    const std::size_t lastIndex = storedData->length - 1;
                    for (std::size_t idx = 0; idx < lastIndex; ++idx) {
                        storedData->data[idx] = (storedData->data[idx + 1] << (8 - tailBits)) | (storedData->data[idx] >> tailBits);
                    }
                    storedData->data[lastIndex] >>= tailBits;
                    if (fillBit == true) {
                        storedData->data[lastIndex] |= (HighByte << (8 - tailBits));
                    }
                }
                else  // If data endian type is DATA_BIG_ENDIAN or if data handling mode type is DATA_MODE_INDEPENDENT.
                {
                    for (std::size_t idx = storedData->length - 1; idx != 0; --idx) {
                        storedData->data[idx] = (storedData->data[idx - 1] << (8 - tailBits)) | (storedData->data[idx] >> tailBits);
                    }
                    storedData->data[0] >>= tailBits;
                    if (fillBit == true) {
                        storedData->data[0] |= (HighByte << (8 - tailBits));
                    }
                }
            }
        }
        return *this;
    }

    // Method that performs round left bit shift by a specified bit offset.
    BinaryDataEngine::BitStreamTransformEngine& BinaryDataEngine::BitStreamTransformEngine::RoundShiftLeft (std::size_t shift) noexcept
    {
        if (*storedData == true && shift != 0)
        {
            if (shift >= Length()) {
                shift %= Length();
            }

            const std::size_t countOfBytesShift = (shift >> 3);
            if (countOfBytesShift > 0) {
                storedData->BytesTransform().RoundShiftLeft(countOfBytesShift);
            }

            const std::size_t tailBits = shift % 8;
            if (tailBits > 0)
            {
                if ((storedData->dataModeType & DATA_MODE_DEPENDENT) != 0U && storedData->dataEndianType == DATA_LITTLE_ENDIAN)
                {
                    const std::byte last = storedData->data[storedData->length - 1];
                    for (std::size_t idx = storedData->length - 1; idx != 0; --idx) {
                        storedData->data[idx] = (storedData->data[idx - 1] >> (8 - tailBits)) | (storedData->data[idx] << tailBits);
                    }
                    storedData->data[0] = (last >> (8 - tailBits) | (storedData->data[0] << tailBits));
                }
                else  // If data endian type is DATA_BIG_ENDIAN or if data handling mode type is DATA_MODE_INDEPENDENT.
                {
                    const std::byte first = storedData->data[0];
                    const std::size_t lastIndex = storedData->length - 1;
                    for (std::size_t idx = 0; idx < lastIndex; ++idx) {
                        storedData->data[idx] = (storedData->data[idx + 1] >> (8 - tailBits)) | (storedData->data[idx] << tailBits);
                    }
                    storedData->data[lastIndex] = (first >> (8 - tailBits) | storedData->data[lastIndex] << tailBits);
                }
            }
        }
        return *this;
    }

    // Method that performs round right bit shift by a specified bit offset.
    BinaryDataEngine::BitStreamTransformEngine& BinaryDataEngine::BitStreamTransformEngine::RoundShiftRight (std::size_t shift) noexcept
    {
        if (*storedData == true && shift != 0)
        {
            if (shift >= Length()) {
                shift %= Length();
            }

            const std::size_t countOfBytesShift = (shift >> 3);
            if (countOfBytesShift > 0) {
                storedData->BytesTransform().RoundShiftRight(countOfBytesShift);
            }

            const std::size_t tailBits = shift % 8;
            if (tailBits > 0)
            {
                if ((storedData->dataModeType & DATA_MODE_DEPENDENT) != 0U && storedData->dataEndianType == DATA_LITTLE_ENDIAN)
                {
                    const std::size_t lastIndex = storedData->length - 1;
                    const std::byte first = storedData->data[lastIndex];
                    for (std::size_t idx = 0; idx < lastIndex; ++idx) {
                        storedData->data[idx] = (storedData->data[idx + 1] << (8 - tailBits)) | (storedData->data[idx] >> tailBits);
                    }
                    storedData->data[lastIndex] = (first << (8 - tailBits) | storedData->data[lastIndex] >> tailBits);
                }
                else  // If data endian type is DATA_BIG_ENDIAN or if data handling mode type is DATA_MODE_INDEPENDENT.
                {
                    const std::byte last = storedData->data[storedData->length - 1];
                    for (std::size_t idx = storedData->length - 1; idx != 0; --idx) {
                        storedData->data[idx] = (storedData->data[idx - 1] << (8 - tailBits)) | (storedData->data[idx] >> tailBits);
                    }
                    storedData->data[0] = (last << (8 - tailBits) | (storedData->data[0] >> tailBits));
                }
            }
        }
        return *this;
    }

    // Method that sets the bit under the specified index to new value.
    const BinaryDataEngine::BitStreamTransformEngine& BinaryDataEngine::BitStreamTransformEngine::Set (const std::size_t index, const bool fillBit) const noexcept
    {
        if (index >= Length()) { return *this; }

        const auto [part, shift] = storedData->bitStreamInformation.GetBitPosition(index);
        if (fillBit == true) {
            storedData->data[part] |= shift;
        }
        else { storedData->data[part] &= ~shift; }
        return *this;
    }

    // Method that reverses a sequence of bits under the specified first/last indexes.
    const BinaryDataEngine::BitStreamTransformEngine& BinaryDataEngine::BitStreamTransformEngine::Reverse (std::size_t first, std::size_t last) const noexcept
    {
        if (last == NPOS) { last = Length() - 1; }
        if (first > last || last >= Length()) { return *this; }

        while (first < last)
        {
            const auto [partFirst, shiftFirst] = storedData->bitStreamInformation.GetBitPosition(first);
            const auto [partLast, shiftLast] = storedData->bitStreamInformation.GetBitPosition(last);

            if (((storedData->data[partFirst] & shiftFirst) != LowByte) !=
                ((storedData->data[partLast] & shiftLast) != LowByte))
            {
                storedData->data[partFirst] ^= shiftFirst;
                storedData->data[partLast] ^= shiftLast;
            }
            first++;
            last--;
        }
        return *this;
    }

    // Method that inverts the bit under the specified index.
    BinaryDataEngine::BitStreamTransformEngine& BinaryDataEngine::BitStreamTransformEngine::Invert (const std::size_t index) noexcept
    {
        if (index >= Length()) { return *this; }

        const auto [part, shift] = storedData->bitStreamInformation.GetBitPosition(index);
        storedData->data[part] ^= shift;
        return *this;
    }

    // Method that inverts the range of bits under the specified first/last indexes.
    BinaryDataEngine::BitStreamTransformEngine& BinaryDataEngine::BitStreamTransformEngine::InvertBlock (std::size_t first, std::size_t last) noexcept
    {
        if (last == NPOS) { last = Length() - 1; }
        if (first > last || last >= Length()) { return *this; }

        while (first <= last) {
            Invert(first++);
        }
        return *this;
    }

    // Bitwise assignment AND operator.
    BinaryDataEngine::BitStreamTransformEngine& BinaryDataEngine::BitStreamTransformEngine::operator&= (const BinaryDataEngine::BitStreamTransformEngine& other) noexcept
    {
        if (*storedData == true)
        {
            auto& currentByteEngine = storedData->BytesTransform();
            const auto& otherByteInformationEngine = other.storedData->BytesInformation();

            // If left operand (current) has data with longer or equal length.
            if (storedData->length >= other.storedData->length)
            {
                for (std::size_t idx = 0; idx < otherByteInformationEngine.Length(); ++idx) {
                    (*currentByteEngine.GetAt(idx)) &= otherByteInformationEngine[idx].value();
                }
                for (std::size_t idx = otherByteInformationEngine.Length(); idx < currentByteEngine.Length(); ++idx) {
                    (*currentByteEngine.GetAt(idx)) &= LowByte;
                }
            }
            // If right operand (other) has data with longer length but data handling mode DATA_MODE_SAFE_OPERATOR is set.
            else if ((storedData->dataModeType & DATA_MODE_SAFE_OPERATOR) != 0U)
            {
                for (std::size_t idx = 0; idx < currentByteEngine.Length(); ++idx) {
                    (*currentByteEngine.GetAt(idx)) &= otherByteInformationEngine[idx].value();
                }
            }
            else  // If right operand (other) has data with longer length and data handling mode is DATA_MODE_UNSAFE_OPERATOR.
            {
                if (storedData->dataEndianType == DATA_LITTLE_ENDIAN)
                {
                    auto newData = system::allocMemoryForArray<std::byte>(other.storedData->length, storedData->data.get(), storedData->length);
                    if (newData != nullptr)
                    {
                        storedData->data = std::move(newData);
                        storedData->length = other.storedData->length;
                        for (std::size_t idx = 0; idx < currentByteEngine.Length(); ++idx) {
                            (*currentByteEngine.GetAt(idx)) &= otherByteInformationEngine[idx].value();
                        }
                    }
                }
                else  // If data endian type is DATA_BIG_ENDIAN.
                {
                    auto newData = system::allocMemoryForArray<std::byte>(other.storedData->length);
                    if (newData != nullptr)
                    {
                        const std::size_t diff = other.storedData->length - storedData->length;
                        memset(newData.get(), 0, diff);
                        memcpy(newData.get() + diff, storedData->data.get(), storedData->length);
                        storedData->data = std::move(newData);
                        storedData->length = other.storedData->length;
                        for (std::size_t idx = 0; idx < currentByteEngine.Length(); ++idx) {
                            (*currentByteEngine.GetAt(idx)) &= otherByteInformationEngine[idx].value();
                        }
                    }
                }
            }
        }
        return *this;
    }

    // Bitwise assignment OR operator.
    BinaryDataEngine::BitStreamTransformEngine& BinaryDataEngine::BitStreamTransformEngine::operator|= (const BinaryDataEngine::BitStreamTransformEngine& other) noexcept
    {
        if (*storedData == true)
        {
            auto& currentByteEngine = storedData->BytesTransform();
            const auto& otherByteInformationEngine = other.storedData->BytesInformation();

            // If left operand (current) has data with longer or equal length.
            if (storedData->length >= other.storedData->length)
            {
                for (std::size_t idx = 0; idx < otherByteInformationEngine.Length(); ++idx) {
                    (*currentByteEngine.GetAt(idx)) |= otherByteInformationEngine[idx].value();
                }
            }
            // If right operand (other) has data with longer length but data handling mode DATA_MODE_SAFE_OPERATOR is set.
            else if ((storedData->dataModeType & DATA_MODE_SAFE_OPERATOR) != 0U)
            {
                for (std::size_t idx = 0; idx < currentByteEngine.Length(); ++idx) {
                    (*currentByteEngine.GetAt(idx)) |= otherByteInformationEngine[idx].value();
                }
            }
            else  // If right operand (other) has data with longer length and data handling mode is DATA_MODE_UNSAFE_OPERATOR.
            {
                if (storedData->dataEndianType == DATA_LITTLE_ENDIAN)
                {
                    auto newData = system::allocMemoryForArray<std::byte>(other.storedData->length, storedData->data.get(), storedData->length);
                    if (newData != nullptr)
                    {
                        storedData->data = std::move(newData);
                        storedData->length = other.storedData->length;
                        for (std::size_t idx = 0; idx < currentByteEngine.Length(); ++idx) {
                            (*currentByteEngine.GetAt(idx)) |= otherByteInformationEngine[idx].value();
                        }
                    }
                }
                else  // If data endian type is DATA_BIG_ENDIAN.
                {
                    auto newData = system::allocMemoryForArray<std::byte>(other.storedData->length);
                    if (newData != nullptr)
                    {
                        const std::size_t diff = other.storedData->length - storedData->length;
                        memset(newData.get(), 0, diff);
                        memcpy(newData.get() + diff, storedData->data.get(), storedData->length);
                        storedData->data = std::move(newData);
                        storedData->length = other.storedData->length;
                        for (std::size_t idx = 0; idx < currentByteEngine.Length(); ++idx) {
                            (*currentByteEngine.GetAt(idx)) |= otherByteInformationEngine[idx].value();
                        }
                    }
                }
            }
        }
        return *this;
    }

    // Bitwise assignment XOR operator.
    BinaryDataEngine::BitStreamTransformEngine& BinaryDataEngine::BitStreamTransformEngine::operator^= (const BinaryDataEngine::BitStreamTransformEngine& other) noexcept
    {
        if (*storedData == true)
        {
            auto& currentByteEngine = storedData->BytesTransform();
            const auto& otherByteInformationEngine = other.storedData->BytesInformation();

            // If left operand (current) has data with longer or equal length.
            if (storedData->length >= other.storedData->length)
            {
                for (std::size_t idx = 0; idx < otherByteInformationEngine.Length(); ++idx) {
                    (*currentByteEngine.GetAt(idx)) ^= otherByteInformationEngine[idx].value();
                }
            }
            // If right operand (other) has data with longer length but data handling mode DATA_MODE_SAFE_OPERATOR is set.
            else if ((storedData->dataModeType & DATA_MODE_SAFE_OPERATOR) != 0U)
            {
                for (std::size_t idx = 0; idx < currentByteEngine.Length(); ++idx) {
                    (*currentByteEngine.GetAt(idx)) ^= otherByteInformationEngine[idx].value();
                }
            }
            else  // If right operand (other) has data with longer length and data handling mode is DATA_MODE_UNSAFE_OPERATOR.
            {
                if (storedData->dataEndianType == DATA_LITTLE_ENDIAN)
                {
                    auto newData = system::allocMemoryForArray<std::byte>(other.storedData->length, storedData->data.get(), storedData->length);
                    if (newData != nullptr)
                    {
                        storedData->data = std::move(newData);
                        storedData->length = other.storedData->length;
                        for (std::size_t idx = 0; idx < currentByteEngine.Length(); ++idx) {
                            (*currentByteEngine.GetAt(idx)) ^= otherByteInformationEngine[idx].value();
                        }
                    }
                }
                else  // If data endian type is DATA_BIG_ENDIAN.
                {
                    auto newData = system::allocMemoryForArray<std::byte>(other.storedData->length);
                    if (newData != nullptr)
                    {
                        const std::size_t diff = other.storedData->length - storedData->length;
                        memset(newData.get(), 0, diff);
                        memcpy(newData.get() + diff, storedData->data.get(), storedData->length);
                        storedData->data = std::move(newData);
                        storedData->length = other.storedData->length;
                        for (std::size_t idx = 0; idx < currentByteEngine.Length(); ++idx) {
                            (*currentByteEngine.GetAt(idx)) ^= otherByteInformationEngine[idx].value();
                        }
                    }
                }
            }
        }
        return *this;
    }

}  // namespace types.
