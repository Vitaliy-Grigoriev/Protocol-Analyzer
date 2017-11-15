// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "../include/analyzer/Common.hpp"
#include "../include/analyzer/System.hpp"
#include "../include/analyzer/RawDataBuffer.hpp"


namespace analyzer::common::types
{
    // Copy constructor.
    RawDataBuffer::RawDataBuffer (const RawDataBuffer& other) noexcept
    {
        data = system::allocMemoryForArray<std::byte>(other.length, other.data.get(), other.length);
        length = other.length;
    }

    // Move assignment constructor.
    RawDataBuffer::RawDataBuffer (RawDataBuffer&& other) noexcept
    {
        if (other.data != nullptr) {
            data = std::move(other.data);
            length = other.length;
            other.Clear();
        }
    }

    // Allocate constructor.
    RawDataBuffer::RawDataBuffer (const std::size_t size) noexcept
    {
        data = system::allocMemoryForArray<std::byte>(size);
        length = size;
    }

    // Copy operator.
    RawDataBuffer& RawDataBuffer::operator= (const RawDataBuffer& other) noexcept
    {
        if (this != &other) {
            data = system::allocMemoryForArray<std::byte>(other.length, other.data.get(), other.length);
            length = other.length;
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

}  // namespace types.
