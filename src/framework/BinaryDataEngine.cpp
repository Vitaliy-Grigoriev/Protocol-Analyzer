// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#include <utility>  // std::move.

#include "../../include/framework/BinaryDataEngine.hpp"


namespace analyzer::framework::common::types
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"

    // Variable that stores system endian.
    inline const DATA_ENDIAN_TYPE BinaryDataEngine::system_endian = CheckSystemEndian();

#pragma clang diagnostic pop


    // Copy constructor of BinaryDataEngine class.
    BinaryDataEngine::BinaryDataEngine (const BinaryDataEngine& other) noexcept
        : bitStreamInformation(this), bitStreamTransform(this),
          byteStreamInformation(this), byteStreamTransform(this)
    {
        if (other == true)
        {
            data = system::allocMemoryForArray<std::byte>(other.length, other.data.get(), other.length);
            if (data != nullptr)
            {
                length = other.length;
                dataModeType = other.dataModeType;
                dataEndianType = other.dataEndianType;
                SetDataModeType(DATA_MODE_ALLOCATION);
            }
        }
    }

    // Move assignment constructor of BinaryDataEngine class.
    BinaryDataEngine::BinaryDataEngine (BinaryDataEngine&& other) noexcept
        : bitStreamInformation(this), bitStreamTransform(this),
          byteStreamInformation(this), byteStreamTransform(this)
    {
        if (other == true)
        {
            data = std::move(other.data);
            length = other.length;
            dataModeType = other.dataModeType;
            dataEndianType = other.dataEndianType;
            other.Clear();
        }
    }

    // Constructor that allocates specified amount of bytes.
    BinaryDataEngine::BinaryDataEngine (const std::size_t size, const uint8_t mode, const DATA_ENDIAN_TYPE endian) noexcept
        : dataModeType(mode), dataEndianType(endian),
          bitStreamInformation(this), bitStreamTransform(this),
          byteStreamInformation(this), byteStreamTransform(this)
    {
        data = system::allocMemoryForArray<std::byte>(size);
        if (data != nullptr)
        {
            if (dataEndianType == DATA_SYSTEM_ENDIAN) {
                dataEndianType = system_endian;
            }
            length = size;
            SetDataModeType(DATA_MODE_ALLOCATION);
            memset(data.get(), 0, length);
        }
    }

    // Constructor that accepts a pointer to allocated binary data.
    BinaryDataEngine::BinaryDataEngine (std::byte* const memory, const std::size_t size, const DATA_ENDIAN_TYPE endian, const uint8_t mode, bool destruct) noexcept
        : data(memory), dataModeType(mode), dataEndianType(endian),
          bitStreamInformation(this), bitStreamTransform(this),
          byteStreamInformation(this), byteStreamTransform(this)
    {
        if (data != nullptr && size != 0)
        {
            if (dataEndianType == DATA_SYSTEM_ENDIAN) {
                dataEndianType = system_endian;
            }
            length = size;
            if (destruct == false) { SetDataModeType(DATA_MODE_NO_ALLOCATION); }
        }
    }

    // Constructor that accepts a pointer to const allocated binary data.
    BinaryDataEngine::BinaryDataEngine (const std::byte* const memory, const std::size_t size, DATA_ENDIAN_TYPE endian, const uint8_t mode) noexcept
        : data(const_cast<std::byte*>(memory)), dataModeType(mode), dataEndianType(endian),
          bitStreamInformation(this), bitStreamTransform(this),
          byteStreamInformation(this), byteStreamTransform(this)
    {
        if (data != nullptr && size != 0)
        {
            if (dataEndianType == DATA_SYSTEM_ENDIAN) {
                dataEndianType = system_endian;
            }
            length = size;
            SetDataModeType(DATA_MODE_NO_ALLOCATION);
        }
    }

    // Copy assignment operator of BinaryDataEngine class.
    BinaryDataEngine& BinaryDataEngine::operator= (const BinaryDataEngine& other) noexcept
    {
        if (this != &other && other == true)
        {
            data = system::allocMemoryForArray<std::byte>(other.length, other.data.get(), other.length);
            if (data != nullptr)
            {
                length = other.length;
                dataModeType = other.dataModeType;
                dataEndianType = other.dataEndianType;
                SetDataModeType(DATA_MODE_ALLOCATION);
            }
        }
        return *this;
    }

    // Move assignment operator of BinaryDataEngine class.
    BinaryDataEngine& BinaryDataEngine::operator= (BinaryDataEngine&& other) noexcept
    {
        if (this != &other && other == true)
        {
            data = std::move(other.data);
            length = other.length;
            dataModeType = other.dataModeType;
            dataEndianType = other.dataEndianType;
            other.Clear();
        }
        return *this;
    }

    // Method that accepts a pointer to allocated binary data.
    bool BinaryDataEngine::AssignReference (std::byte* const memory, const std::size_t size, const bool destruct) noexcept
    {
        data.reset(memory);
        if (data != nullptr)
        {
            length = size;
            if (destruct == false) { SetDataModeType(DATA_MODE_NO_ALLOCATION); }
            return true;
        }
        return false;
    }

    // Method that changes handling mode type of stored data in BinaryDataEngine class.
    void BinaryDataEngine::SetDataModeType (const uint8_t mode) const noexcept
    {
        if ((mode & DATA_MODE_DEPENDENT) != 0U) {
            dataModeType &= ~DATA_MODE_INDEPENDENT;
            dataModeType |= DATA_MODE_DEPENDENT;
        }
        else if ((mode & DATA_MODE_INDEPENDENT) != 0U) {
            dataModeType &= ~DATA_MODE_DEPENDENT;
            dataModeType |= DATA_MODE_INDEPENDENT;
        }

        if ((mode & DATA_MODE_SAFE_OPERATOR) != 0U) {
            dataModeType &= ~DATA_MODE_UNSAFE_OPERATOR;
            dataModeType |= DATA_MODE_SAFE_OPERATOR;
        }
        else if ((mode & DATA_MODE_UNSAFE_OPERATOR) != 0U) {
            dataModeType &= ~DATA_MODE_SAFE_OPERATOR;
            dataModeType |= DATA_MODE_UNSAFE_OPERATOR;
        }

        if ((mode & DATA_MODE_ALLOCATION) != 0U) {
            dataModeType &= ~DATA_MODE_NO_ALLOCATION;
            dataModeType |= DATA_MODE_ALLOCATION;
        }
        else if ((mode & DATA_MODE_NO_ALLOCATION) != 0U) {
            dataModeType &= ~DATA_MODE_ALLOCATION;
            dataModeType |= DATA_MODE_NO_ALLOCATION;
        }

        if ((mode & DATA_MODE_OPERATOR_ALIGN_LOW_ORDER) != 0U) {
            dataModeType &= ~DATA_MODE_OPERATOR_ALIGN_HIGH_ORDER;
            dataModeType |= DATA_MODE_OPERATOR_ALIGN_LOW_ORDER;
        }
        else if ((mode & DATA_MODE_OPERATOR_ALIGN_HIGH_ORDER) != 0U) {
            dataModeType &= ~DATA_MODE_OPERATOR_ALIGN_LOW_ORDER;
            dataModeType |= DATA_MODE_OPERATOR_ALIGN_HIGH_ORDER;
        }
    }

    // Method that changes handling mode type of stored data in BinaryDataEngine class.
    void BinaryDataEngine::SetDataEndianType (const DATA_ENDIAN_TYPE endian, const bool convert) noexcept
    {
        if (dataEndianType == endian) { return; }

        if (convert == true)
        {
            if ((endian == DATA_BIG_ENDIAN && dataEndianType == DATA_LITTLE_ENDIAN) ||
                (endian == DATA_LITTLE_ENDIAN && dataEndianType == DATA_BIG_ENDIAN))
            {
                for (std::size_t idx = 0; idx < length / 2; ++idx)
                {
                    data[idx] ^= data[length - idx - 1];
                    data[length - idx - 1] ^= data[idx];
                    data[idx] ^= data[length - idx - 1];
                }
            }
            else if ((endian == DATA_BIG_ENDIAN && dataEndianType == DATA_REVERSE_BIG_ENDIAN) ||
                     (endian == DATA_REVERSE_BIG_ENDIAN && dataEndianType == DATA_BIG_ENDIAN))
            {
                for (std::size_t idx = 0; idx < length; ++idx)
                {
                    data[idx] = (data[idx] & HighPartByte) >> 4 | (data[idx] & LowPartByte) << 4;
                    data[idx] = (data[idx] & HighBitsInHalvesByte) >> 2 | (data[idx] & LowBitsInHalvesByte) << 2;
                    data[idx] = (data[idx] & HighAlternateByte) >> 1 | (data[idx] & LowAlternateByte) << 1;
                }
            }
            else if ((endian == DATA_LITTLE_ENDIAN && dataEndianType == DATA_REVERSE_BIG_ENDIAN) ||
                     (endian == DATA_REVERSE_BIG_ENDIAN && dataEndianType == DATA_LITTLE_ENDIAN))
            {
                if (length == 1)
                {
                    data[0] = (data[0] & HighPartByte) >> 4 | (data[0] & LowPartByte) << 4;
                    data[0] = (data[0] & HighBitsInHalvesByte) >> 2 | (data[0] & LowBitsInHalvesByte) << 2;
                    data[0] = (data[0] & HighAlternateByte) >> 1 | (data[0] & LowAlternateByte) << 1;
                }
                else
                {
                    for (std::size_t idx = 0; idx < length / 2; ++idx)
                    {
                        data[idx] = (data[idx] & HighPartByte) >> 4 | (data[idx] & LowPartByte) << 4;
                        data[idx] = (data[idx] & HighBitsInHalvesByte) >> 2 | (data[idx] & LowBitsInHalvesByte) << 2;
                        data[idx] = (data[idx] & HighAlternateByte) >> 1 | (data[idx] & LowAlternateByte) << 1;
                        data[length - idx - 1] = (data[length - idx - 1] & HighPartByte) >> 4 | (data[length - idx - 1] & LowPartByte) << 4;
                        data[length - idx - 1] = (data[length - idx - 1] & HighBitsInHalvesByte) >> 2 | (data[length - idx - 1] & LowBitsInHalvesByte) << 2;
                        data[length - idx - 1] = (data[length - idx - 1] & HighAlternateByte) >> 1 | (data[length - idx - 1] & LowAlternateByte) << 1;
                        data[idx] ^= data[length - idx - 1];
                        data[length - idx - 1] ^= data[idx];
                        data[idx] ^= data[length - idx - 1];
                    }
                }
            }
        }
        dataEndianType = endian;
    }

    // Safety getter of internal value.
    std::byte* BinaryDataEngine::GetAt (const std::size_t index) const noexcept
    {
        return (index < length ? &data[index] : nullptr);
    }

    // Method that clears the internal binary data.
    void BinaryDataEngine::Clear(void) noexcept
    {
        if ((dataModeType & DATA_MODE_NO_ALLOCATION) != 0U) {
            [[maybe_unused]] auto unused = data.release();
        }
        data.reset(nullptr);
        length = 0;
    }

    // Method that resets the internal state of BinaryDataEngine class to default state.
    void BinaryDataEngine::Reset(void) noexcept
    {
        Clear();
        dataModeType = DATA_MODE_DEFAULT;
        dataEndianType = system_endian;
    }

    // Method that returns internal binary data represented in hex string.
    std::string BinaryDataEngine::ToHexString(void) const noexcept
    {
        return common::text::getHexString(data.get(), length);
    }

    // Method returns internal data in standard uint8_t type.
    std::optional<uint8_t> BinaryDataEngine::ToU8Bit(void) const noexcept
    {
        return bitStreamInformation.Convert<uint8_t>();
    }

    // Method returns internal data in standard uint16_t type.
    std::optional<uint16_t> BinaryDataEngine::ToU16Bit(void) const noexcept
    {
        return bitStreamInformation.Convert<uint16_t>();
    }

    // Method returns internal data in standard uint32_t type.
    std::optional<uint32_t> BinaryDataEngine::ToU32Bit(void) const noexcept
    {
        return bitStreamInformation.Convert<uint32_t>();
    }

    // Method returns internal data in standard uint64_t type.
    std::optional<uint64_t> BinaryDataEngine::ToU64Bit(void) const noexcept
    {
        return bitStreamInformation.Convert<uint64_t>();
    }

    // Method returns internal data in standard int8_t type.
    std::optional<int8_t> BinaryDataEngine::To8Bit(void) const noexcept
    {
        return bitStreamInformation.Convert<int8_t>();
    }

    // Method returns internal data in standard int16_t type.
    std::optional<int16_t> BinaryDataEngine::To16Bit(void) const noexcept
    {
        return bitStreamInformation.Convert<int16_t>();
    }

    // Method returns internal data in standard int32_t type.
    std::optional<int32_t> BinaryDataEngine::To32Bit(void) const noexcept
    {
        return bitStreamInformation.Convert<int32_t>();
    }

    // Method returns internal data in standard int64_t type.
    std::optional<int64_t> BinaryDataEngine::To64Bit(void) const noexcept
    {
        return bitStreamInformation.Convert<int64_t>();
    }

    // Operator that returns a const reference to an element by selected index.
    std::optional<std::byte> BinaryDataEngine::operator[] (const std::size_t index) const noexcept
    {
        if (index >= length) { return std::nullopt; }
        return data[index];
    }

    // Bitwise assignment AND operator.
    BinaryDataEngine& BinaryDataEngine::operator&= (const BinaryDataEngine& other) noexcept
    {
        if (dataEndianType == other.dataEndianType && length == other.length)
        {
            for (std::size_t idx = 0; idx < length; )
            {
                if (idx + sizeof(uint32_t) <= length)
                {
                    (*reinterpret_cast<uint32_t*>(&data[idx])) ^= (*reinterpret_cast<const uint32_t*>(other.GetAt(idx)));
                    idx += sizeof(uint32_t);
                }
                else
                {
                    data[idx] &= other[idx].value();
                    ++idx;
                }
            }
        }
        else { bitStreamTransform &= other.BitsInformation(); }
        return *this;
    }

    // Bitwise assignment OR operator.
    BinaryDataEngine& BinaryDataEngine::operator|= (const BinaryDataEngine& other) noexcept
    {
        if (dataEndianType == other.dataEndianType && length == other.length)
        {
            for (std::size_t idx = 0; idx < length; )
            {
                if (idx + sizeof(uint32_t) <= length)
                {
                    (*reinterpret_cast<uint32_t*>(&data[idx])) |= (*reinterpret_cast<const uint32_t*>(other.GetAt(idx)));
                    idx += sizeof(uint32_t);
                }
                else
                {
                    data[idx] |= other[idx].value();
                    ++idx;
                }
            }
        }
        else { bitStreamTransform |= other.BitsInformation(); }
        return *this;
    }

    // Bitwise assignment XOR operator.
    BinaryDataEngine& BinaryDataEngine::operator^= (const BinaryDataEngine& other) noexcept
    {
        if (dataEndianType == other.dataEndianType && length == other.length)
        {
            for (std::size_t idx = 0; idx < length; )
            {
                if (idx + sizeof(uint32_t) <= length)
                {
                    (*reinterpret_cast<uint32_t*>(&data[idx])) ^= (*reinterpret_cast<const uint32_t*>(other.GetAt(idx)));
                    idx += sizeof(uint32_t);
                }
                else
                {
                    data[idx] ^= other[idx].value();
                    ++idx;
                }
            }
        }
        else { bitStreamTransform ^= other.BitsInformation(); }
        return *this;
    }

    // Literal operator that converts number in string representation to 8-bit integer in little endian form.
    std::optional<BinaryDataEngine> operator"" _u8_le (const char* value, const std::size_t size) noexcept
    {
        char* end = nullptr;
        const uint64_t number = strtoull(value, &end, 10);
        if (value + size == end && number <= UINT8_MAX)
        {
            const uint8_t temp = static_cast<uint8_t>(number);
            BinaryDataEngine data(sizeof(uint8_t), DATA_MODE_DEFAULT);
            if (data.AssignData<uint8_t>(&temp, 1) == true)
            {
                data.SetDataEndianType(DATA_LITTLE_ENDIAN);
                return data;
            }
        }
        return std::nullopt;
    }

    // Literal operator that converts number in string representation to 8-bit integer in big endian form.
    std::optional<BinaryDataEngine> operator"" _u8_be (const char* value, const std::size_t size) noexcept
    {
        char* end = nullptr;
        const uint64_t number = strtoull(value, &end, 10);
        if (value + size == end && number <= UINT8_MAX)
        {
            const uint8_t temp = static_cast<uint8_t>(number);
            BinaryDataEngine data(sizeof(uint8_t), DATA_MODE_DEFAULT);
            if (data.AssignData<uint8_t>(&temp, 1) == true)
            {
                data.SetDataEndianType(DATA_BIG_ENDIAN);
                return data;
            }
        }
        return std::nullopt;
    }

    // Literal operator that converts number in string representation to 8-bit integer in reverse big endian form.
    std::optional<BinaryDataEngine> operator"" _u8_rbe (const char* value, const std::size_t size) noexcept
    {
        char* end = nullptr;
        const uint64_t number = strtoull(value, &end, 10);
        if (value + size == end && number <= UINT8_MAX)
        {
            const uint8_t temp = static_cast<uint8_t>(number);
            BinaryDataEngine data(sizeof(uint8_t), DATA_MODE_DEFAULT);
            if (data.AssignData<uint8_t>(&temp, 1) == true)
            {
                data.SetDataEndianType(DATA_REVERSE_BIG_ENDIAN);
                return data;
            }
        }
        return std::nullopt;
    }

    // Literal operator that converts number in string representation to 16-bit integer in little endian form.
    std::optional<BinaryDataEngine> operator"" _u16_le (const char* value, const std::size_t size) noexcept
    {
        char* end = nullptr;
        const uint64_t number = strtoull(value, &end, 10);
        if (value + size == end && number <= UINT16_MAX)
        {
            const uint16_t temp = static_cast<uint16_t>(number);
            BinaryDataEngine data(sizeof(uint16_t), DATA_MODE_DEFAULT);
            if (data.AssignData<uint16_t>(&temp, 1) == true)
            {
                data.SetDataEndianType(DATA_LITTLE_ENDIAN);
                return data;
            }
        }
        return std::nullopt;
    }

    // Literal operator that converts number in string representation to 16-bit integer in big endian form.
    std::optional<BinaryDataEngine> operator"" _u16_be (const char* value, const std::size_t size) noexcept
    {
        char* end = nullptr;
        const uint64_t number = strtoull(value, &end, 10);
        if (value + size == end && number <= UINT16_MAX)
        {
            const uint16_t temp = static_cast<uint16_t>(number);
            BinaryDataEngine data(sizeof(uint16_t), DATA_MODE_DEFAULT);
            if (data.AssignData<uint16_t>(&temp, 1) == true)
            {
                data.SetDataEndianType(DATA_BIG_ENDIAN);
                return data;
            }
        }
        return std::nullopt;
    }

    // Literal operator that converts number in string representation to 16-bit integer in reverse big endian form.
    std::optional<BinaryDataEngine> operator"" _u16_rbe (const char* value, const std::size_t size) noexcept
    {
        char* end = nullptr;
        const uint64_t number = strtoull(value, &end, 10);
        if (value + size == end && number <= UINT16_MAX)
        {
            const uint16_t temp = static_cast<uint16_t>(number);
            BinaryDataEngine data(sizeof(uint16_t), DATA_MODE_DEFAULT);
            if (data.AssignData<uint16_t>(&temp, 1) == true)
            {
                data.SetDataEndianType(DATA_REVERSE_BIG_ENDIAN);
                return data;
            }
        }
        return std::nullopt;
    }

    // Literal operator that converts number in string representation to 32-bit integer in little endian form.
    std::optional<BinaryDataEngine> operator"" _u32_le (const char* value, const std::size_t size) noexcept
    {
        char* end = nullptr;
        const uint64_t number = strtoull(value, &end, 10);
        if (value + size == end && number <= UINT32_MAX)
        {
            const uint32_t temp = static_cast<uint32_t>(number);
            BinaryDataEngine data(sizeof(uint32_t), DATA_MODE_DEFAULT);
            if (data.AssignData<uint32_t>(&temp, 1) == true)
            {
                data.SetDataEndianType(DATA_LITTLE_ENDIAN);
                return data;
            }
        }
        return std::nullopt;
    }

    // Literal operator that converts number in string representation to 32-bit integer in big endian form.
    std::optional<BinaryDataEngine> operator"" _u32_be (const char* value, const std::size_t size) noexcept
    {
        char* end = nullptr;
        const uint64_t number = strtoull(value, &end, 10);
        if (value + size == end && number <= UINT32_MAX)
        {
            const uint32_t temp = static_cast<uint32_t>(number);
            BinaryDataEngine data(sizeof(uint32_t), DATA_MODE_DEFAULT);
            if (data.AssignData<uint32_t>(&temp, 1) == true)
            {
                data.SetDataEndianType(DATA_BIG_ENDIAN);
                return data;
            }
        }
        return std::nullopt;
    }

    // Literal operator that converts number in string representation to 32-bit integer in reverse big endian form.
    std::optional<BinaryDataEngine> operator"" _u32_rbe (const char* value, const std::size_t size) noexcept
    {
        char* end = nullptr;
        const uint64_t number = strtoull(value, &end, 10);
        if (value + size == end && number <= UINT32_MAX)
        {
            const uint32_t temp = static_cast<uint32_t>(number);
            BinaryDataEngine data(sizeof(uint32_t), DATA_MODE_DEFAULT);
            if (data.AssignData<uint32_t>(&temp, 1) == true)
            {
                data.SetDataEndianType(DATA_REVERSE_BIG_ENDIAN);
                return data;
            }
        }
        return std::nullopt;
    }

    // Literal operator that converts number in string representation to 64-bit integer in little endian form.
    std::optional<BinaryDataEngine> operator"" _u64_le (const char* value, const std::size_t size) noexcept
    {
        char* end = nullptr;
        const uint64_t number = strtoull(value, &end, 10);
        if (value + size == end && errno != ERANGE)
        {
            BinaryDataEngine data(sizeof(uint64_t), DATA_MODE_DEFAULT);
            if (data.AssignData<uint64_t>(&number, 1) == true)
            {
                data.SetDataEndianType(DATA_LITTLE_ENDIAN);
                return data;
            }
        }
        return std::nullopt;
    }

    // Literal operator that converts number in string representation to 64-bit integer in big endian form.
    std::optional<BinaryDataEngine> operator"" _u64_be (const char* value, const std::size_t size) noexcept
    {
        char* end = nullptr;
        const uint64_t number = strtoull(value, &end, 10);
        if (value + size == end && errno != ERANGE)
        {
            BinaryDataEngine data(sizeof(uint64_t), DATA_MODE_DEFAULT);
            if (data.AssignData<uint64_t>(&number, 1) == true)
            {
                data.SetDataEndianType(DATA_BIG_ENDIAN);
                return data;
            }
        }
        return std::nullopt;
    }

    // Literal operator that converts number in string representation to 64-bit integer in reverse big endian form.
    std::optional<BinaryDataEngine> operator"" _u64_rbe (const char* value, const std::size_t size) noexcept
    {
        char* end = nullptr;
        const uint64_t number = strtoull(value, &end, 10);
        if (value + size == end && errno != ERANGE)
        {
            BinaryDataEngine data(sizeof(uint64_t), DATA_MODE_DEFAULT);
            if (data.AssignData<uint64_t>(&number, 1) == true)
            {
                data.SetDataEndianType(DATA_REVERSE_BIG_ENDIAN);
                return data;
            }
        }
        return std::nullopt;
    }

}  // namespace types.
