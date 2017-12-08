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
        if (this != &other)
        {
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

    // Safety getter of internal value.
    std::byte* RawDataBuffer::GetAt (const std::size_t index) const noexcept
    {
        return index < length ? &data[index] : nullptr;
    }

    // Check system endian.
    bool RawDataBuffer::CheckSystemEndian(void) const noexcept
    {
        const uint8_t endianness[2] = { 1, 0 };
        const uint16_t t = *reinterpret_cast<const uint16_t*>(endianness);
        return (t & 0x100) == 0;
    }



    /* ***************************************************************************************************** */
    /* ****************************************** BitStreamEngine ****************************************** */

    /**
     * @fn static void leftRoundBytesShift (std::byte *, const std::byte *, std::byte *) noexcept;
     * @brief Support function that performs left byte round shift to the selected byte.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence (the element following the last one).
     * @param [in] newHead - Byte to which the left round shift is performed.
     */
    static void leftRoundBytesShift (std::byte* head, const std::byte* end, std::byte* newHead) noexcept
    {
        if (head >= end || newHead <= head || newHead >= end) { return; }
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
     * @param [in] end - End position of the byte sequence (the element following the last one).
     * @param [in] newEnd - Byte to which the right round shift is performed.
     */
    static void rightRoundBytesShift (const std::byte* head, std::byte* end, std::byte* newEnd) noexcept
    {
        if (head >= end || newEnd < head || newEnd >= --end) { return; }
        std::byte* prev = newEnd;
        const std::byte* const rend = std::prev(head);

        while (end != prev)
        {
            std::swap(*prev--, *end--);
            if (prev == rend) {
                prev = newEnd;
            } else if (end == newEnd) {
                newEnd = prev;
            }
        }
    }

    /**
     * @fn static void leftDirectBytesShift (std::byte *, const std::byte *, std::byte *, bool) noexcept;
     * @brief Support function that performs left byte direct shift to the selected byte.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence (the element following the last one).
     * @param [in] newHead - Byte to which the left direct shift is performed.
     * @param [in] fillBit - Boolean flag that indicates about the value of the offset fill. Default: false (0x00).
     */
    static void leftDirectBytesShift (std::byte* head, const std::byte* end, std::byte* newHead, bool fillBit = false) noexcept
    {
        if (head >= end || newHead <= head || newHead >= end) { return; }
        std::byte* next = newHead;

        while (next != end)
        {
            *head++ = *next++;
            if (head == newHead) {
                newHead = next;
            }
        }
        memset(head, (fillBit == false ? 0x00 : 0xFF), static_cast<std::size_t>(end - head));
    }

    /**
     * @fn static void rightDirectBytesShift (std::byte *, std::byte *, std::byte *, bool) noexcept;
     * @brief Support function that performs right byte direct shift to the selected byte.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence (the element following the last one).
     * @param [in] newEnd - Byte to which the right direct shift is performed.
     * @param [in] fillBit - Boolean flag that indicates about the value of the offset fill. Default: false (0x00).
     */
    static void rightDirectBytesShift (std::byte* head, std::byte* end, std::byte* newEnd, bool fillBit = false) noexcept
    {
        if (head >= end || newEnd < head || newEnd >= --end) { return; }
        std::byte* prev = newEnd;
        const std::byte* const rend = std::prev(head);

        while (prev != rend)
        {
            *end-- = *prev--;
            if (end == newEnd) {
                newEnd = prev;
            }
        }
        memset(head, (fillBit == false ? 0x00 : 0xFF), static_cast<std::size_t>(end - prev));
    }

    /**
     * @fn static inline void leftRoundBytesShift (std::byte *, const std::byte *, const std::size_t) noexcept;
     * @brief Support function that performs left byte round shift by a specified byte offset.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence (the element following the last one).
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
     * @param [in] end - End position of the byte sequence (the element following the last one).
     * @param [in] shift - Byte offset for round right shift.
     */
    static inline void rightRoundBytesShift (const std::byte* head, std::byte* end, const std::size_t shift) noexcept
    {
        rightRoundBytesShift(head, end, end - shift - 1);
    }

    /**
     * @fn static inline void leftDirectBytesShift (std::byte *, const std::byte *, const std::size_t, bool) noexcept;
     * @brief Support function that performs left byte direct shift by a specified byte offset.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence (the element following the last one).
     * @param [in] shift - Byte offset for direct left shift.
     * @param [in] fillBit - Boolean flag that indicates about the value of the offset fill. Default: false (0x00).
     */
    static inline void leftDirectBytesShift (std::byte* head, const std::byte* end, const std::size_t shift, bool fillBit = false) noexcept
    {
        leftDirectBytesShift(head, end, head + shift, fillBit);
    }

    /**
     * @fn static inline void rightDirectBytesShift (std::byte *, std::byte *, const std::size_t, bool) noexcept;
     * @brief Support function that performs right byte direct shift by a specified byte offset.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence (the element following the last one).
     * @param [in] shift - Byte offset for direct right shift.
     * @param [in] fillBit - Boolean flag that indicates about the value of the offset fill. Default: false (0x00).
     */
    static inline void rightDirectBytesShift (std::byte* head, std::byte* end, const std::size_t shift, bool fillBit = false) noexcept
    {
        rightDirectBytesShift(head, end, end - shift - 1, fillBit);
    }




    const RawDataBuffer::BitStreamEngine& RawDataBuffer::BitStreamEngine::ShiftLeft (const std::size_t shift, bool fillBit) const noexcept
    {
        if (storedData.data != nullptr && storedData.length > 0 && shift > 0)
        {
            const std::size_t size = Length();
            if (shift >= size) {
                memset(storedData.data.get(), (fillBit == false ? 0x00 : 0xFF), storedData.length);
                return *this;
            }

            const std::size_t countOfBytesShift = shift / 8;
            if (countOfBytesShift > 0) {
                leftDirectBytesShift(storedData.data.get(), storedData.data.get() + storedData.length, countOfBytesShift, fillBit);
            }

            const std::size_t tailBits = shift % 8;
            if (tailBits > 0)
            {
                for (std::size_t idx = 0; idx < storedData.Size() - 1; ++idx) {
                    storedData.data[idx] = (storedData.data[idx + 1] >> (8 - tailBits)) | (storedData.data[idx] << tailBits);
                }

                std::byte* const last = &storedData.data[storedData.Size() - 1];
                if (fillBit == false) {
                    *last <<= tailBits;
                } else {
                    *last = (std::byte(0xFF) >> (8 - tailBits)) | (*last << tailBits);
                }
            }
        }
        return *this;
    }


    const RawDataBuffer::BitStreamEngine& RawDataBuffer::BitStreamEngine::ShiftRight (const std::size_t shift, bool fillBit) const noexcept
    {
        if (storedData.data != nullptr && storedData.length > 0 && shift > 0)
        {

        }
        return *this;
    }


    const RawDataBuffer::BitStreamEngine& RawDataBuffer::BitStreamEngine::RoundShiftLeft (const std::size_t shift) const noexcept
    {
        if (storedData.data != nullptr && storedData.length > 0)
        {

        }
        return *this;
    }


    const RawDataBuffer::BitStreamEngine& RawDataBuffer::BitStreamEngine::RoundShiftRight (const std::size_t shift) const noexcept
    {
        if (storedData.data != nullptr && storedData.length > 0)
        {

        }
        return *this;
    }


    bool RawDataBuffer::BitStreamEngine::Test (const std::size_t index) const noexcept
    {
        if (index >= Length()) { return false; }
        const std::size_t part = index >> 3; // Division by eight (8).
        const std::byte shift = std::byte(0x80) >> (index % 8);
        return ((storedData.data[part] & shift) != std::byte(0x00));
    }


}  // namespace types.
