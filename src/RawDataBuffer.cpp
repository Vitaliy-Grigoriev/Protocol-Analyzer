// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "../include/analyzer/Common.hpp"
#include "../include/analyzer/System.hpp"
#include "../include/analyzer/RawDataBuffer.hpp"


namespace analyzer::common::types
{
    // Copy constructor.
    RawDataBuffer::RawDataBuffer (const RawDataBuffer& other) noexcept
            : bitStream(*this)
    {
        data = system::allocMemoryForArray<std::byte>(other.length, other.data.get(), other.length);
        if (data != nullptr) {
            length = other.length;
        }
    }

    // Move assignment constructor.
    RawDataBuffer::RawDataBuffer (RawDataBuffer&& other) noexcept
            : bitStream(*this)
    {
        if (other.data != nullptr) {
            data = std::move(other.data);
            length = other.length;
            other.Clear();
        }
    }

    // Allocate constructor.
    RawDataBuffer::RawDataBuffer (const std::size_t size) noexcept
            : bitStream(*this)
    {
        data = system::allocMemoryForArray<std::byte>(size);
        if (data != nullptr) {
            length = size;
        }
    }

    // Copy operator.
    RawDataBuffer& RawDataBuffer::operator= (const RawDataBuffer& other) noexcept
    {
        if (this != &other) {
            data = system::allocMemoryForArray<std::byte>(other.length, other.data.get(), other.length);
            if (data != nullptr) {
                length = other.length;
            }
        }
        return *this;
    }

    // Move assignment operator.
    RawDataBuffer& RawDataBuffer::operator= (RawDataBuffer&& other) noexcept
    {
        if (this != &other)
        {
            data = std::move(other.data);
            length = other.length;
            other.Clear();
        }
        return *this;
    }

    // Clear internal data buffer.
    void RawDataBuffer::Clear(void) noexcept
    {
        data.reset(nullptr);
        length = 0;
    }

    // Check system endian.
    bool RawDataBuffer::CheckSystemEndian(void) const noexcept
    {
        const uint8_t endianness[2] = { 1, 0 };
        const uint16_t t = *reinterpret_cast<const uint16_t*>(endianness);
        return (t & 0x100) == 0;
    }

    std::byte* RawDataBuffer::GetAt (const std::size_t index) const noexcept
    {
        return index < length ? &data[index] : nullptr;
    }


    void RawDataBuffer::BitStreamEngine::shiftLeft (std::byte* bits, const std::size_t size, std::size_t shift, bool isRound) const noexcept
    {

    }


    const RawDataBuffer::BitStreamEngine& RawDataBuffer::BitStreamEngine::ShiftLeft (const std::size_t shift) const noexcept
    {
        shiftLeft(storedData.data.get(), storedData.length, shift, false);
        return *this;
    }

    const RawDataBuffer::BitStreamEngine& RawDataBuffer::BitStreamEngine::ShiftRight (const std::size_t shift) const noexcept
    {
        //shiftLeft(storedData.data.get(), storedData.length, shift, false);
        return *this;
    }

}  // namespace types.
