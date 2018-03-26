// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#include "../include/analyzer/Common.hpp"
#include "../include/analyzer/System.hpp"
#include "../include/analyzer/RawDataBuffer.hpp"


namespace analyzer::common::types
{
    /* ****************************************************************************************************** */
    /* ******************************************** RawDataBuffer ******************************************* */

    // Copy constructor.
    RawDataBuffer::RawDataBuffer (const RawDataBuffer& other) noexcept
            : bitStreamTransform(*this), byteStreamTransform(*this)
    {
        data = system::allocMemoryForArray<std::byte>(other.length, other.data.get(), other.length);
        if (data != nullptr) {
            length = other.length;
            dataMode = other.dataMode;
            dataEndian = other.dataEndian;
        }
    }

    // Move assignment constructor.
    RawDataBuffer::RawDataBuffer (RawDataBuffer&& other) noexcept
            : bitStreamTransform(*this), byteStreamTransform(*this)
    {
        if (other.data != nullptr) {
            data = std::move(other.data);
            length = other.length;
            dataMode = other.dataMode;
            dataEndian = other.dataEndian;
            other.Clear();
        }
    }

    // Allocated constructor.
    RawDataBuffer::RawDataBuffer (const std::size_t size, DATA_HANDLING_MODE mode, DATA_ENDIAN_TYPE endian) noexcept
            : dataMode(mode), dataEndian(endian), bitStreamTransform(*this), byteStreamTransform(*this)
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
                dataMode = other.dataMode;
                dataEndian = other.dataEndian;
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
            dataMode = other.dataMode;
            dataEndian = other.dataEndian;
            other.Clear();
        }
        return *this;
    }

    // Method that changes handling mode type of stored data in RawDataBuffer class.
    void RawDataBuffer::SetDataEndianType (DATA_ENDIAN_TYPE endian, bool convert) noexcept
    {
        dataEndian = endian;
        if (convert == true)
        {
            for (std::size_t idx = 0; idx < length / 2; ++idx)
            {
                data[idx] = data[idx] ^ data[length - idx - 1];
                data[length - idx - 1] = data[length - idx - 1] ^ data[idx];
                data[idx] = data[idx] ^ data[length - idx - 1];
            }
        }
    }

    // Safety getter of internal value.
    std::byte* RawDataBuffer::GetAt (const std::size_t index) const noexcept
    {
        return index < length ? &data[index] : nullptr;
    }

    // Clear internal data buffer.
    void RawDataBuffer::Clear(void) noexcept
    {
        data.reset(nullptr);
        length = 0;
        dataMode = DATA_MODE_DEPENDENT;
        dataEndian = CheckSystemEndian();
    }

    /* ******************************************** RawDataBuffer ******************************************* */
    /* ****************************************************************************************************** */


    /* ****************************************************************************************************** */
    /* *********************************************** Support ********************************************** */

    /**
     * @fn static void leftRoundBytesShiftBE (std::byte *, const std::byte *, std::byte *) noexcept;
     * @brief Support function that performs left byte round shift to the selected byte.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence (the element following the last one).
     * @param [in] newHead - Byte to which the left round byte shift is performed.
     */
    static void leftRoundBytesShiftBE (std::byte* head, const std::byte* end, std::byte* newHead) noexcept
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
     * @fn static void rightRoundBytesShiftBE (const std::byte *, std::byte *, std::byte *) noexcept;
     * @brief Support function that performs right byte round shift to the selected byte.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence (the element following the last one).
     * @param [in] newEnd - Byte to which the right round byte shift is performed.
     */
    static void rightRoundBytesShiftBE (const std::byte* head, std::byte* end, std::byte* newEnd) noexcept
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
     * @fn static void leftDirectBytesShiftBE (std::byte *, const std::byte *, std::byte *, const std::byte) noexcept;
     * @brief Support function that performs left byte direct shift to the selected byte in big-endian format.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence (the element following the last one).
     * @param [in] newHead - Byte to which the left direct byte shift is performed.
     * @param [in] fillByte - Value of the fill byte after the left byte shift. Default: 0x00.
     */
    static void leftDirectBytesShiftBE (std::byte* head, const std::byte* end, std::byte* newHead, const std::byte fillByte = std::byte(0x00)) noexcept
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
        memset(head, static_cast<int32_t>(fillByte), static_cast<std::size_t>(end - head));
    }

    /**
     * @fn static void rightDirectBytesShiftBE (std::byte *, std::byte *, std::byte *, const std::byte) noexcept;
     * @brief Support function that performs right byte direct shift to the selected byte in big-endian format.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence (the element following the last one).
     * @param [in] newEnd - Byte to which the right direct byte shift is performed.
     * @param [in] fillByte - Value of the fill byte after the right byte shift. Default: 0x00.
     */
    static void rightDirectBytesShiftBE (std::byte* head, std::byte* end, std::byte* newEnd, const std::byte fillByte = std::byte(0x00)) noexcept
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
        memset(head, static_cast<int32_t>(fillByte), static_cast<std::size_t>(end - prev));
    }

    /**
     * @fn static inline void leftRoundBytesShiftBE (std::byte *, const std::byte *, const std::size_t) noexcept;
     * @brief Support function that performs left byte round shift by a specified byte offset in big-endian format.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence (the element following the last one).
     * @param [in] shift - Byte offset for round left byte shift.
     */
    static inline void leftRoundBytesShiftBE (std::byte* head, const std::byte* end, const std::size_t shift) noexcept
    {
        leftRoundBytesShiftBE(head, end, head + shift);
    }

    /**
     * @fn static inline void leftRoundBytesShiftLE (const std::byte *, std::byte *, const std::size_t) noexcept;
     * @brief Support function that performs left byte round shift by a specified byte offset in little-endian format.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence (the element following the last one).
     * @param [in] shift - Byte offset for round left byte shift.
     */
    static inline void leftRoundBytesShiftLE (const std::byte* head, std::byte* end, const std::size_t shift) noexcept
    {
        rightRoundBytesShiftBE(head, end, end - shift - 1);
    }

    /**
     * @fn static inline void rightRoundBytesShiftBE (const std::byte *, std::byte *, const std::size_t) noexcept;
     * @brief Support function that performs right byte round shift by a specified byte offset in big-endian format.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence (the element following the last one).
     * @param [in] shift - Byte offset for round right byte shift.
     */
    static inline void rightRoundBytesShiftBE (const std::byte* head, std::byte* end, const std::size_t shift) noexcept
    {
        rightRoundBytesShiftBE(head, end, end - shift - 1);
    }

    /**
     * @fn static inline void rightRoundBytesShiftLE (std::byte *, const std::byte *, const std::size_t) noexcept;
     * @brief Support function that performs right byte round shift by a specified byte offset in little-endian format.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence (the element following the last one).
     * @param [in] shift - Byte offset for round right byte shift.
     */
    static inline void rightRoundBytesShiftLE (std::byte* head, const std::byte* end, const std::size_t shift) noexcept
    {
        leftRoundBytesShiftBE(head, end, head + shift);
    }

    /**
     * @fn static inline void leftDirectBytesShiftBE (std::byte *, const std::byte *, const std::size_t, const std::byte) noexcept;
     * @brief Support function that performs left byte direct shift by a specified byte offset in big-endian format.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence (the element following the last one).
     * @param [in] shift - Byte offset for direct left byte shift.
     * @param [in] fillByte - Value of the fill byte after the left direct byte shift. Default: 0x00.
     */
    static inline void leftDirectBytesShiftBE (std::byte* head, const std::byte* end, const std::size_t shift, const std::byte fillByte = std::byte(0x00)) noexcept
    {
        leftDirectBytesShiftBE(head, end, head + shift, fillByte);
    }

    /**
     * @fn static inline void leftDirectBytesShiftLE (std::byte *, std::byte *, const std::size_t, const std::byte) noexcept;
     * @brief Support function that performs left byte direct shift by a specified byte offset in little-endian format.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence (the element following the last one).
     * @param [in] shift - Byte offset for direct left byte shift.
     * @param [in] fillByte - Value of the fill byte after the left direct byte shift. Default: 0x00.
     */
    static inline void leftDirectBytesShiftLE (std::byte* head, std::byte* end, const std::size_t shift, const std::byte fillByte = std::byte(0x00)) noexcept
    {
        rightDirectBytesShiftBE(head, end, end - shift - 1, fillByte);
    }

    /**
     * @fn static inline void rightDirectBytesShiftBE (std::byte *, std::byte *, const std::size_t, const std::byte) noexcept;
     * @brief Support function that performs right byte direct shift by a specified byte offset in big-endian format.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence (the element following the last one).
     * @param [in] shift - Byte offset for direct right shift.
     * @param [in] fillByte - Value of the fill byte after the right direct byte shift. Default: 0x00.
     */
    static inline void rightDirectBytesShiftBE (std::byte* head, std::byte* end, const std::size_t shift, const std::byte fillByte = std::byte(0x00)) noexcept
    {
        rightDirectBytesShiftBE(head, end, end - shift - 1, fillByte);
    }

    /**
     * @fn static inline void rightDirectBytesShiftLE (std::byte *, const std::byte *, const std::size_t, const std::byte) noexcept;
     * @brief Support function that performs right byte direct shift by a specified byte offset in little-endian format.
     * @param [in] head - Start position of the byte sequence.
     * @param [in] end - End position of the byte sequence (the element following the last one).
     * @param [in] shift - Byte offset for direct right shift.
     * @param [in] fillByte - Value of the fill byte after the right direct byte shift. Default: 0x00.
     */
    static inline void rightDirectBytesShiftLE (std::byte* head, const std::byte* end, const std::size_t shift, const std::byte fillByte = std::byte(0x00)) noexcept
    {
        leftDirectBytesShiftBE(head, end, head + shift, fillByte);
    }

    /* *********************************************** Support ********************************************** */
    /* ****************************************************************************************************** */


    /* ***************************************************************************************************** */
    /* ****************************************** BitStreamEngine ****************************************** */

    // Method that returns the correct position of selected bit in stored raw data in any data endian.
    std::pair<std::size_t, std::byte> RawDataBuffer::BitStreamEngine::GetBitPosition (const std::size_t index) const noexcept
    {
        if (index >= Length()) { std::pair(0, 0); }

        if (storedData.dataMode == DATA_MODE_DEPENDENT && storedData.dataEndian == DATA_LITTLE_ENDIAN)
        {
            return std::pair(index >> 3, std::byte(0x01) << index % 8);
        }
        if (storedData.dataMode == DATA_MODE_DEPENDENT && storedData.dataEndian == DATA_BIG_ENDIAN)
        {
            return std::pair(storedData.length - (index >> 3) - 1, std::byte(0x01) << index % 8);
        }
        // If data handling mode is DATA_MODE_INDEPENDENT.
        return std::pair(index >> 3, std::byte(0x80) >> index % 8);
    }

    // Method that performs direct left bit shift by a specified bit offset.
    const RawDataBuffer::BitStreamEngine& RawDataBuffer::BitStreamEngine::ShiftLeft (const std::size_t shift, bool fillBit) const noexcept
    {
        if (storedData.data != nullptr && storedData.length > 0 && shift != 0)
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
                if (storedData.dataMode == DATA_MODE_DEPENDENT && storedData.dataEndian == DATA_LITTLE_ENDIAN)
                {
                    for (std::size_t idx = storedData.length - 1; idx != 0; --idx) {
                        storedData.data[idx] = (storedData.data[idx - 1] >> (8 - tailBits)) | (storedData.data[idx] << tailBits);
                    }
                    storedData.data[0] <<= tailBits;
                    if (fillBit == true) {
                        storedData.data[0] |= (std::byte(0xFF) >> (8 - tailBits));
                    }
                }
                else  // If data endian type is DATA_BIG_ENDIAN or if data handling mode is DATA_MODE_INDEPENDENT.
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
    const RawDataBuffer::BitStreamEngine& RawDataBuffer::BitStreamEngine::ShiftRight (const std::size_t shift, bool fillBit) const noexcept
    {
        if (storedData.data != nullptr && storedData.length > 0 && shift != 0)
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
                if (storedData.dataMode == DATA_MODE_DEPENDENT && storedData.dataEndian == DATA_LITTLE_ENDIAN)
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
                else  // If data endian type is DATA_BIG_ENDIAN or if data handling mode is DATA_MODE_INDEPENDENT.
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
    const RawDataBuffer::BitStreamEngine& RawDataBuffer::BitStreamEngine::RoundShiftLeft (std::size_t shift) const noexcept
    {
        if (storedData.data != nullptr && storedData.length > 0 && shift != 0)
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
                if (storedData.dataMode == DATA_MODE_DEPENDENT && storedData.dataEndian == DATA_LITTLE_ENDIAN)
                {
                    const std::byte last = storedData.data[storedData.length - 1];
                    for (std::size_t idx = storedData.length - 1; idx != 0; --idx) {
                        storedData.data[idx] = (storedData.data[idx - 1] >> (8 - tailBits)) | (storedData.data[idx] << tailBits);
                    }
                    storedData.data[0] = (last >> (8 - tailBits) | (storedData.data[0] << tailBits));
                }
                else  // If data endian type is DATA_BIG_ENDIAN or if data handling mode is DATA_MODE_INDEPENDENT.
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
    const RawDataBuffer::BitStreamEngine& RawDataBuffer::BitStreamEngine::RoundShiftRight (std::size_t shift) const noexcept
    {
        if (storedData.data != nullptr && storedData.length > 0 && shift != 0)
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
                if (storedData.dataMode == DATA_MODE_DEPENDENT && storedData.dataEndian == DATA_LITTLE_ENDIAN)
                {
                    const std::size_t lastIndex = storedData.length - 1;
                    const std::byte first = storedData.data[lastIndex];
                    for (std::size_t idx = 0; idx < lastIndex; ++idx) {
                        storedData.data[idx] = (storedData.data[idx + 1] << (8 - tailBits)) | (storedData.data[idx] >> tailBits);
                    }
                    storedData.data[lastIndex] = (first << (8 - tailBits) | storedData.data[lastIndex] >> tailBits);
                }
                else  // If data endian type is DATA_BIG_ENDIAN or if data handling mode is DATA_MODE_INDEPENDENT.
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
    bool RawDataBuffer::BitStreamEngine::Test (const std::size_t index) const noexcept
    {
        if (index >= Length()) { return false; }

        const auto [part, shift] = GetBitPosition(index);
        return ((storedData.data[part] & shift) != std::byte(0x00));
    }

    // Method that returns bit sequence characteristic when all bit are set in block of stored data.
    bool RawDataBuffer::BitStreamEngine::All (std::size_t first, std::size_t last) const noexcept
    {
        if (last == npos) { last = Length() - 1; }
        if (first > last || last >= Length()) { return false; }

        while (first <= last)
        {
            if (Test(first++) == false) {
                return false;
            }
        }
        return true;
    }

    // Method that returns bit sequence characteristic when any of the bits are set in block of stored data.
    bool RawDataBuffer::BitStreamEngine::Any (std::size_t first, std::size_t last) const noexcept
    {
        if (last == npos) { last = Length() - 1; }
        if (first > last || last >= Length()) { return false; }

        while (first <= last)
        {
            if (Test(first++) == true) {
                return true;
            }
        }
        return false;
    }

    // Method that returns bit sequence characteristic when none of the bits are set in block of stored data.
    bool RawDataBuffer::BitStreamEngine::None (std::size_t first, std::size_t last) const noexcept
    {
        if (last == npos) { last = Length() - 1; }
        if (first > last || last >= Length()) { return false; }

        while (first <= last)
        {
            if (Test(first++) == true) {
                return false;
            }
        }
        return true;
    }

    // Method that returns the number of bits that are set in the selected interval of stored data.
    std::size_t RawDataBuffer::BitStreamEngine::Count (std::size_t first, std::size_t last) const noexcept
    {
        if (last == npos) { last = Length() - 1; }
        if (first > last || last >= Length()) { return npos; }

        std::size_t count = 0;
        while (first <= last) {
            count += (Test(first++) == true) ? 1 : 0;
        }
        return count;
    }

    // Method that returns position of the first set bit in the selected interval of stored data.
    std::size_t RawDataBuffer::BitStreamEngine::GetFirstIndex (const std::size_t first, std::size_t last, bool isRelative) const noexcept
    {
        if (last == npos) { last = Length() - 1; }
        if (first > last || last >= Length()) { return npos; }

        std::size_t index = first;
        while (index <= last)
        {
            if (Test(index) == true) {
                return ((isRelative == true) ? index - first : index);
            }
            index++;
        }
        return npos;
    }

    // Method that returns position of the last set bit in the selected interval of stored data.
    std::size_t RawDataBuffer::BitStreamEngine::GetLastIndex (const std::size_t first, std::size_t last, bool isRelative) const noexcept
    {
        if (last == npos) { last = Length() - 1; }
        if (first > last || last >= Length()) { return npos; }

        std::size_t index = last;
        while (index >= first)
        {
            if (Test(index) == true) {
                return ((isRelative == true) ? index - first : index);
            }
            index--;
        }
        return npos;
    }

    // Method that sets the bit under the specified index to new value.
    const RawDataBuffer::BitStreamEngine& RawDataBuffer::BitStreamEngine::Set (const std::size_t index, bool fillBit) const noexcept
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
    const RawDataBuffer::BitStreamEngine& RawDataBuffer::BitStreamEngine::Reverse (std::size_t first, std::size_t last) const noexcept
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
    const RawDataBuffer::BitStreamEngine& RawDataBuffer::BitStreamEngine::Invert (const std::size_t index) const noexcept
    {
        if (index >= Length()) { return *this; }

        const auto [part, shift] = GetBitPosition(index);
        storedData.data[part] ^= shift;
        return *this;
    }

    // Method that inverts the range of bits under the specified first/last indexes.
    const RawDataBuffer::BitStreamEngine& RawDataBuffer::BitStreamEngine::InvertBlock (std::size_t first, std::size_t last) const noexcept
    {
        if (last == npos) { last = Length() - 1; }
        if (first > last || last >= Length()) { return *this; }

        while (first <= last) {
            Invert(first++);
        }
        return *this;
    }

    // Method that outputs internal binary raw data in string format.
    std::string RawDataBuffer::BitStreamEngine::ToString (std::size_t first, std::size_t last) const noexcept
    {
        if (last == npos) { last = Length() - 1; }
        if (first > last || last >= Length()) { return std::string(); }

        std::ostringstream result;
        if (storedData.IsEmpty() == false)
        {
            result.unsetf(std::ios_base::boolalpha);
            if (storedData.dataMode == DATA_MODE_DEPENDENT && storedData.dataEndian == DATA_LITTLE_ENDIAN)
            {
                while (last >= first && last != 0) {
                    result << Test(last--);
                }
                if (last == 0) { result << Test(0); }
            }
            else  // If data endian type is DATA_BIG_ENDIAN or if data handling mode is DATA_MODE_INDEPENDENT.
            {
                while (first <= last) {
                    result << Test(first++);
                }
            }
        }
        return result.str();
    }

    // Method that outputs internal binary raw data in short string format.
    std::string RawDataBuffer::BitStreamEngine::ToShortString (std::size_t first, std::size_t last, uint16_t compression) const noexcept
    {
        if (compression == 0) { return ToString(first, last); }
        if (last == npos) { last = Length() - 1; }
        if (first > last || last >= Length()) { return std::string(); }

        std::ostringstream result;
        if (storedData.IsEmpty() == false)
        {
            // Do first compression on fly.
            result.unsetf(std::ios_base::boolalpha);
            if (storedData.dataMode == DATA_MODE_DEPENDENT && storedData.dataEndian == DATA_LITTLE_ENDIAN)
            {
                std::size_t count = 1;
                bool lastValue = Test(last--);
                while (last >= first && last != 0)
                {
                    if (Test(last--) == lastValue) {
                        count++;
                    }
                    else
                    {
                        if (count == 1) {
                            result << lastValue;
                            lastValue = !lastValue;
                        } else {
                            result << '(' << lastValue << "){" << count << '}';
                            count = 1;
                            lastValue = !lastValue;
                        }
                    }
                }
                if (last == 0)
                {
                    if (Test(last) == lastValue) {
                        count++;
                        result << '(' << lastValue << "){" << count << '}';
                    }
                    else
                    {
                        if (count == 1) {
                            result << lastValue;
                        } else {
                            result << '(' << lastValue << "){" << count << '}';
                        }
                        result << !lastValue;
                    }
                }
                else if (last == first - 1)
                {
                    if (count == 1) {
                        result << lastValue;
                    } else {
                        result << '(' << lastValue << "){" << count << '}';
                    }
                }

                for (std::size_t idx = 2; idx < compression; ++idx)
                {

                }
            }
            else  // If data endian type is DATA_BIG_ENDIAN or if data handling mode is DATA_MODE_INDEPENDENT.
            {
                while (first <= last) {
                    result << Test(first++);
                }
            }

            // Do second compression.

        }
        return result.str();
    }

    // Logical assignment bitwise AND operator that transforms internal binary raw data.
    const RawDataBuffer::BitStreamEngine& RawDataBuffer::BitStreamEngine::operator&= (const RawDataBuffer::BitStreamEngine& other) const noexcept
    {
        const auto lambda = [] (std::byte* dst, std::size_t size, const std::byte* src) noexcept -> void
        {

        };
        return *this;
    }

    // Logical assignment bitwise OR operator that transforms internal binary raw data.
    const RawDataBuffer::BitStreamEngine& RawDataBuffer::BitStreamEngine::operator|= (const RawDataBuffer::BitStreamEngine& other) const noexcept
    {
        const auto lambda = [] (std::byte* dst, std::size_t size, const std::byte* src) noexcept -> void
        {

        };
        return *this;
    }

    // Logical assignment bitwise XOR operator that transforms internal binary raw data.
    const RawDataBuffer::BitStreamEngine& RawDataBuffer::BitStreamEngine::operator^= (const RawDataBuffer::BitStreamEngine& other) const noexcept
    {
        const auto lambda = [] (std::byte* dst, std::size_t size, const std::byte* src) noexcept -> void
        {

        };
        return *this;
    }


    /* ****************************************** BitStreamEngine ****************************************** */
    /* ***************************************************************************************************** */


    /* ****************************************************************************************************** */
    /* ****************************************** ByteStreamEngine ****************************************** */

    // Method that performs direct left byte shift by a specified byte offset.
    const RawDataBuffer::ByteStreamEngine& RawDataBuffer::ByteStreamEngine::ShiftLeft (const std::size_t shift, const std::byte fillByte) const noexcept
    {
        if (storedData.data != nullptr && storedData.length > 0 && shift != 0)
        {
            if (shift >= storedData.length) {
                memset(storedData.data.get(), static_cast<int32_t>(fillByte), storedData.length);
                return *this;
            }

            if (storedData.dataMode == DATA_MODE_DEPENDENT && storedData.dataEndian == DATA_LITTLE_ENDIAN) {
                leftDirectBytesShiftLE(storedData.data.get(), storedData.data.get() + storedData.length, shift, fillByte);
            }
            else {  // If data endian type is DATA_BIG_ENDIAN or if data handling mode is DATA_MODE_INDEPENDENT.
                leftDirectBytesShiftBE(storedData.data.get(), storedData.data.get() + storedData.length, shift, fillByte);
            }
        }
        return *this;
    }

    // Method that performs direct right byte shift by a specified byte offset.
    const RawDataBuffer::ByteStreamEngine& RawDataBuffer::ByteStreamEngine::ShiftRight (const std::size_t shift, const std::byte fillByte) const noexcept
    {
        if (storedData.data != nullptr && storedData.length > 0 && shift != 0)
        {
            if (shift >= storedData.length) {
                memset(storedData.data.get(), static_cast<int32_t>(fillByte), storedData.length);
                return *this;
            }

            if (storedData.dataMode == DATA_MODE_DEPENDENT && storedData.dataEndian == DATA_LITTLE_ENDIAN) {
                rightDirectBytesShiftLE(storedData.data.get(), storedData.data.get() + storedData.length, shift, fillByte);
            }
            else {   // If data endian type is DATA_BIG_ENDIAN or if data handling mode is DATA_MODE_INDEPENDENT.
                rightDirectBytesShiftBE(storedData.data.get(), storedData.data.get() + storedData.length, shift, fillByte);
            }
        }
        return *this;
    }

    // Method that performs round left bit shift by a specified byte offset.
    const RawDataBuffer::ByteStreamEngine& RawDataBuffer::ByteStreamEngine::RoundShiftLeft (std::size_t shift) const noexcept
    {
        if (storedData.data != nullptr && storedData.length > 0 && shift != 0)
        {
            if (shift >= storedData.length) {
                shift %= storedData.length;
            }

            if (storedData.dataMode == DATA_MODE_DEPENDENT && storedData.dataEndian == DATA_LITTLE_ENDIAN) {
                leftRoundBytesShiftLE(storedData.data.get(), storedData.data.get() + storedData.length, shift);
            }
            else {  // If data endian type is DATA_BIG_ENDIAN or if data handling mode is DATA_MODE_INDEPENDENT.
                leftRoundBytesShiftBE(storedData.data.get(), storedData.data.get() + storedData.length, shift);
            }
        }
        return *this;
    }

    // Method that performs round right bit shift by a specified byte offset.
    const RawDataBuffer::ByteStreamEngine& RawDataBuffer::ByteStreamEngine::RoundShiftRight (std::size_t shift) const noexcept
    {
        if (storedData.data != nullptr && storedData.length > 0 && shift != 0)
        {
            if (shift >= storedData.length) {
                shift %= storedData.length;
            }

            if (storedData.dataMode == DATA_MODE_DEPENDENT && storedData.dataEndian == DATA_LITTLE_ENDIAN) {
                rightRoundBytesShiftLE(storedData.data.get(), storedData.data.get() + storedData.length, shift);
            }
            else {  // If data endian type is DATA_BIG_ENDIAN or if data handling mode is DATA_MODE_INDEPENDENT.
                rightRoundBytesShiftBE(storedData.data.get(), storedData.data.get() + storedData.length, shift);
            }
        }
        return *this;
    }

    /* ****************************************** ByteStreamEngine ****************************************** */
    /* ****************************************************************************************************** */

}  // namespace types.
