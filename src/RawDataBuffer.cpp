// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "../include/analyzer/Common.hpp"
#include "../include/analyzer/System.hpp"
#include "../include/analyzer/RawDataBuffer.hpp"


namespace analyzer::common::types
{
    // Copy constructor.
    RawDataBuffer::RawDataBuffer (const RawDataBuffer& other) noexcept
            : bitStreamTransform(*this)
    {
        data = system::allocMemoryForArray<std::byte>(other.length, other.data.get(), other.length);
        if (data != nullptr) {
            length = other.length;
        }
    }

    // Move assignment constructor.
    RawDataBuffer::RawDataBuffer (RawDataBuffer&& other) noexcept
            : bitStreamTransform(*this)
    {
        if (other.data != nullptr) {
            data = std::move(other.data);
            length = other.length;
            other.Clear();
        }
    }

    // Allocate constructor.
    RawDataBuffer::RawDataBuffer (const std::size_t size) noexcept
            : bitStreamTransform(*this)
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


    /*************************************** BitStreamEngine ***************************************/

    /**
     * @fn static void leftRoundBytesShift (std::byte *, const std::byte *, std::byte *) noexcept;
     * @brief Support function that performs left byte round shift to the selected byte.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence.
     * @param [in] newHead - Byte to which the left shift is performed.
     */
    static void leftRoundBytesShift (std::byte* head, const std::byte* end, std::byte* newHead) noexcept
    {
        if (head >= end || newHead < head || newHead > end) { return; }
        std::byte* next = newHead;
        while (head != next)
        {
            std::swap(*head++, *next++);
            if (next == end) {
                next = newHead;
            } else if (head == newHead) {
                newHead = next;
            }
        }
    }

    /**
     * @fn static void rightRoundBytesShift (const std::byte *, std::byte *, std::byte *) noexcept;
     * @brief Support function that performs right byte round shift to the selected byte.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence.
     * @param [in] newEnd - Byte to which the right shift is performed.
     */
    static void rightRoundBytesShift (const std::byte* head, std::byte* end, std::byte* newEnd) noexcept
    {
        if (head >= end || newEnd < head || newEnd > end) { return; }
        end--;
        std::byte* prev = newEnd;
        while (end != prev)
        {
            std::swap(*prev--, *end--);
            if (prev == std::prev(head)) {
                prev = newEnd;
            } else if (end == newEnd) {
                newEnd = prev;
            }
        }
    }

    /**
     * @fn static inline void leftRoundBytesShift (const std::byte *, std::byte *, const std::size_t) noexcept;
     * @brief Support function that performs left byte round shift by a specified byte offset.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence.
     * @param [in] shift - Byte offset for round left shift.
     */
    static inline void leftRoundBytesShift (std::byte* head, const std::byte* end, const std::size_t shift) noexcept
    {
        leftRoundBytesShift(head, end, head + shift);
    }

    /**
     * @fn static inline void rightRoundBytesShift (const std::byte *, std::byte *, const std::size_t) noexcept;
     * @brief Support function that performs right byte round shift by a specified byte offset.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence.
     * @param [in] shift - Byte offset for round right shift.
     */
    static inline void rightRoundBytesShift (const std::byte* head, std::byte* end, const std::size_t shift) noexcept
    {
        rightRoundBytesShift(head, end, end - shift - 1);
    }



    const RawDataBuffer::BitStreamEngine& RawDataBuffer::BitStreamEngine::ShiftLeft (const std::size_t shift) const noexcept
    {
        //shiftLeft(storedData.data.get(), storedData.length, shift, false);
        return *this;
    }

    const RawDataBuffer::BitStreamEngine& RawDataBuffer::BitStreamEngine::ShiftRight (const std::size_t shift) const noexcept
    {
        //shiftLeft(storedData.data.get(), storedData.length, shift, false);
        return *this;
    }

    const RawDataBuffer::BitStreamEngine & RawDataBuffer::BitStreamEngine::RoundShiftLeft (const std::size_t shift) const noexcept
    {
        return *this;
    }

    const RawDataBuffer::BitStreamEngine & RawDataBuffer::BitStreamEngine::RoundShiftRight (const std::size_t shift) const noexcept
    {
        return *this;
    }


}  // namespace types.
