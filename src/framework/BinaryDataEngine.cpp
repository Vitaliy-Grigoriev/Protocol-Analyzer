// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#include <utility>  // std::move.

#include "../../include/framework/BinaryDataEngine.hpp"


namespace analyzer::common::types
{
    // Variable that stores system endian.
    inline const DATA_ENDIAN_TYPE BinaryDataEngine::system_endian = CheckSystemEndian();


    // Copy constructor of BinaryDataEngine class.
    BinaryDataEngine::BinaryDataEngine (const BinaryDataEngine& other) noexcept
            : bitStreamTransform(*this), byteStreamTransform(*this)
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
            : bitStreamTransform(*this), byteStreamTransform(*this)
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
            : dataModeType(mode), dataEndianType(endian), bitStreamTransform(*this), byteStreamTransform(*this)
    {
        data = system::allocMemoryForArray<std::byte>(size);
        if (data != nullptr) {
            length = size;
            SetDataModeType(DATA_MODE_ALLOCATION);
            memset(data.get(), 0, length);
        }
    }

    // Constructor that accepts a pointer to allocated binary data.
    BinaryDataEngine::BinaryDataEngine (std::byte* const memory, const std::size_t size, const DATA_ENDIAN_TYPE endian, const uint8_t mode, bool destruct) noexcept
            : data(memory), dataModeType(mode), dataEndianType(endian), bitStreamTransform(*this), byteStreamTransform(*this)
    {
        if (data != nullptr && size != 0) {
            length = size;
            if (destruct == false) { SetDataModeType(DATA_MODE_NO_ALLOCATION); }
        }
    }

    // Copy assignment operator of BinaryDataEngine class.
    BinaryDataEngine& BinaryDataEngine::operator= (const BinaryDataEngine& other) noexcept
    {
        if (this != &other && other == true)
        {
            data = system::allocMemoryForArray<std::byte>(other.length, other.data.get(), other.length);
            if (data != nullptr) {
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
        if (data != nullptr) {
            length = size;
            if (destruct == false) { SetDataModeType(DATA_MODE_NO_ALLOCATION); }
            return true;
        }
        return false;
    }

    // Method that changes handling mode type of stored data in BinaryDataEngine class.
    void BinaryDataEngine::SetDataModeType (const uint8_t mode) noexcept
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
        dataEndianType = endian;

        if (convert == true)
        {
            for (std::size_t idx = 0; idx < length / 2; ++idx)
            {
                data[idx] ^= data[length - idx - 1];
                data[length - idx - 1] ^= data[idx];
                data[idx] ^= data[length - idx - 1];
            }
        }
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

    // Operator that returns a const reference to an element by selected index.
    inline std::optional<std::byte> BinaryDataEngine::operator[] (const std::size_t index) const noexcept
    {
        if (index >= length) { return std::nullopt; }
        return data[index];
    }

    // Logical assignment bitwise AND operator that transforms internal binary data.
    const BinaryDataEngine& BinaryDataEngine::operator&= (const BinaryDataEngine& other) const noexcept
    {
        bitStreamTransform &= other.BitsTransform();
        return *this;
    }

    // Logical assignment bitwise OR operator that transforms internal binary data.
    const BinaryDataEngine& BinaryDataEngine::operator|= (const BinaryDataEngine& other) const noexcept
    {
        bitStreamTransform |= other.BitsTransform();
        return *this;
    }

    // Logical assignment bitwise XOR operator that transforms internal binary data.
    const BinaryDataEngine& BinaryDataEngine::operator^= (const BinaryDataEngine& other) const noexcept
    {
        bitStreamTransform ^= other.BitsTransform();
        return *this;
    }

}  // namespace types.
