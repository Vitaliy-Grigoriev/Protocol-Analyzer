// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#include <utility>  // std::swap.

#include "../../include/framework/BinaryDataEngine.hpp"


namespace analyzer::framework::common::types
{
    /* ************************************************************************************************************* */
    /* ************************************************** Support ************************************************** */

    /**
     * @fn static void leftRoundBytesShiftBE (std::byte *, const std::byte *, std::byte *) noexcept;
     * @brief Support function that performs left byte round shift to the selected byte.
     * @param [in] head - Pointer to start position in byte sequence.
     * @param [in] end - Pointer to end position in byte sequence (the element following the last one).
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
     * @param [in] head - Pointer to start position in byte sequence.
     * @param [in] end - Pointer to end position in byte sequence (the element following the last one).
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
     * @param [in] head - Pointer to start position in byte sequence.
     * @param [in] end - Pointer to end position in byte sequence (the element following the last one).
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
     * @param [in] head - Pointer to start position in byte sequence.
     * @param [in] end - Pointer to end position in byte sequence (the element following the last one).
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
     * @param [in] shift - Byte offset for left round byte shift.
     */
    static inline void leftRoundBytesShiftBE (std::byte* head, const std::byte* end, const std::size_t shift) noexcept
    {
        leftRoundBytesShiftBE(head, end, head + shift);
    }

    /**
     * @fn static inline void leftRoundBytesShiftLE (const std::byte *, std::byte *, const std::size_t) noexcept;
     * @brief Support function that performs left byte round shift by a specified byte offset in little-endian format.
     * @param [in] head - Pointer to start position in byte sequence.
     * @param [in] end - Pointer to end position in byte sequence (the element following the last one).
     * @param [in] shift - Byte offset for left round byte shift.
     */
    static inline void leftRoundBytesShiftLE (const std::byte* head, std::byte* end, const std::size_t shift) noexcept
    {
        rightRoundBytesShiftBE(head, end, end - shift - 1);
    }

    /**
     * @fn static inline void rightRoundBytesShiftBE (const std::byte *, std::byte *, const std::size_t) noexcept;
     * @brief Support function that performs right byte round shift by a specified byte offset in big-endian format.
     * @param [in] head - Pointer to start position in byte sequence.
     * @param [in] end - Pointer to end position in byte sequence (the element following the last one).
     * @param [in] shift - Byte offset for right round byte shift.
     */
    static inline void rightRoundBytesShiftBE (const std::byte* head, std::byte* end, const std::size_t shift) noexcept
    {
        rightRoundBytesShiftBE(head, end, end - shift - 1);
    }

    /**
     * @fn static inline void rightRoundBytesShiftLE (std::byte *, const std::byte *, const std::size_t) noexcept;
     * @brief Support function that performs right byte round shift by a specified byte offset in little-endian format.
     * @param [in] head - Pointer to start position in byte sequence.
     * @param [in] end - Pointer to end position in byte sequence (the element following the last one).
     * @param [in] shift - Byte offset for right round byte shift.
     */
    static inline void rightRoundBytesShiftLE (std::byte* head, const std::byte* end, const std::size_t shift) noexcept
    {
        leftRoundBytesShiftBE(head, end, head + shift);
    }

    /**
     * @fn static inline void leftDirectBytesShiftBE (std::byte *, const std::byte *, const std::size_t, const std::byte) noexcept;
     * @brief Support function that performs left byte direct shift by a specified byte offset in big-endian format.
     * @param [in] head - Pointer to start position in byte sequence.
     * @param [in] end - Pointer to end position in byte sequence (the element following the last one).
     * @param [in] shift - Byte offset for left direct byte shift.
     * @param [in] fillByte - Value of the fill byte after the left direct byte shift. Default: 0x00.
     */
    static inline void leftDirectBytesShiftBE (std::byte* head, const std::byte* end, const std::size_t shift, const std::byte fillByte = std::byte(0x00)) noexcept
    {
        leftDirectBytesShiftBE(head, end, head + shift, fillByte);
    }

    /**
     * @fn static inline void leftDirectBytesShiftLE (std::byte *, std::byte *, const std::size_t, const std::byte) noexcept;
     * @brief Support function that performs left byte direct shift by a specified byte offset in little-endian format.
     * @param [in] head - Pointer to start position in byte sequence.
     * @param [in] end - Pointer to end position in byte sequence (the element following the last one).
     * @param [in] shift - Byte offset for left direct byte shift.
     * @param [in] fillByte - Value of the fill byte after the left direct byte shift. Default: 0x00.
     */
    static inline void leftDirectBytesShiftLE (std::byte* head, std::byte* end, const std::size_t shift, const std::byte fillByte = std::byte(0x00)) noexcept
    {
        rightDirectBytesShiftBE(head, end, end - shift - 1, fillByte);
    }

    /**
     * @fn static inline void rightDirectBytesShiftBE (std::byte *, std::byte *, const std::size_t, const std::byte) noexcept;
     * @brief Support function that performs right byte direct shift by a specified byte offset in big-endian format.
     * @param [in] head - Pointer to start position in byte sequence.
     * @param [in] end - Pointer to end position in byte sequence (the element following the last one).
     * @param [in] shift - Byte offset for right direct byte shift.
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
     * @param [in] shift - Byte offset for right direct byte shift.
     * @param [in] fillByte - Value of the fill byte after the right direct byte shift. Default: 0x00.
     */
    static inline void rightDirectBytesShiftLE (std::byte* head, const std::byte* end, const std::size_t shift, const std::byte fillByte = std::byte(0x00)) noexcept
    {
        leftDirectBytesShiftBE(head, end, head + shift, fillByte);
    }

    /* ************************************************** Support ************************************************** */
    /* ************************************************************************************************************* */


    // Method that returns the correct position of selected byte in stored data in any data endian.
    std::size_t BinaryDataEngine::ByteStreamEngine::GetBytePosition (const std::size_t index) const noexcept
    {
        if (storedData.dataEndianType == DATA_LITTLE_ENDIAN || (storedData.dataModeType & DATA_MODE_INDEPENDENT) != 0U)
        {
            return index;
        }
        // If data endian type is DATA_BIG_ENDIAN.
        return storedData.length - index - 1;
    }

    // Method that performs direct left byte shift by a specified byte offset.
    const BinaryDataEngine::ByteStreamEngine& BinaryDataEngine::ByteStreamEngine::ShiftLeft (const std::size_t shift, const std::byte fillByte) const noexcept
    {
        if (storedData == true && shift != 0)
        {
            if (shift >= storedData.length) {
                memset(storedData.data.get(), static_cast<int32_t>(fillByte), storedData.length);
                return *this;
            }

            if ((storedData.dataModeType & DATA_MODE_DEPENDENT) != 0U && storedData.dataEndianType == DATA_LITTLE_ENDIAN) {
                leftDirectBytesShiftLE(storedData.data.get(), storedData.data.get() + storedData.length, shift, fillByte);
            }
            else {  // If data endian type is DATA_BIG_ENDIAN or if data handling mode type is DATA_MODE_INDEPENDENT.
                leftDirectBytesShiftBE(storedData.data.get(), storedData.data.get() + storedData.length, shift, fillByte);
            }
        }
        return *this;
    }

    // Method that performs direct right byte shift by a specified byte offset.
    const BinaryDataEngine::ByteStreamEngine& BinaryDataEngine::ByteStreamEngine::ShiftRight (const std::size_t shift, const std::byte fillByte) const noexcept
    {
        if (storedData == true && shift != 0)
        {
            if (shift >= storedData.length) {
                memset(storedData.data.get(), static_cast<int32_t>(fillByte), storedData.length);
                return *this;
            }

            if ((storedData.dataModeType & DATA_MODE_DEPENDENT) != 0U && storedData.dataEndianType == DATA_LITTLE_ENDIAN) {
                rightDirectBytesShiftLE(storedData.data.get(), storedData.data.get() + storedData.length, shift, fillByte);
            }
            else {  // If data endian type is DATA_BIG_ENDIAN or if data handling mode type is DATA_MODE_INDEPENDENT.
                rightDirectBytesShiftBE(storedData.data.get(), storedData.data.get() + storedData.length, shift, fillByte);
            }
        }
        return *this;
    }

    // Method that performs round left bit shift by a specified byte offset.
    const BinaryDataEngine::ByteStreamEngine& BinaryDataEngine::ByteStreamEngine::RoundShiftLeft (std::size_t shift) const noexcept
    {
        if (storedData == true && shift != 0)
        {
            if (shift >= storedData.length) {
                shift %= storedData.length;
            }

            if ((storedData.dataModeType & DATA_MODE_DEPENDENT) != 0U && storedData.dataEndianType == DATA_LITTLE_ENDIAN) {
                leftRoundBytesShiftLE(storedData.data.get(), storedData.data.get() + storedData.length, shift);
            }
            else {  // If data endian type is DATA_BIG_ENDIAN or if data handling mode type is DATA_MODE_INDEPENDENT.
                leftRoundBytesShiftBE(storedData.data.get(), storedData.data.get() + storedData.length, shift);
            }
        }
        return *this;
    }

    // Method that performs round right bit shift by a specified byte offset.
    const BinaryDataEngine::ByteStreamEngine& BinaryDataEngine::ByteStreamEngine::RoundShiftRight (std::size_t shift) const noexcept
    {
        if (storedData == true && shift != 0)
        {
            if (shift >= storedData.length) {
                shift %= storedData.length;
            }

            if ((storedData.dataModeType & DATA_MODE_DEPENDENT) != 0U && storedData.dataEndianType == DATA_LITTLE_ENDIAN) {
                rightRoundBytesShiftLE(storedData.data.get(), storedData.data.get() + storedData.length, shift);
            }
            else {  // If data endian type is DATA_BIG_ENDIAN or if data handling mode type is DATA_MODE_INDEPENDENT.
                rightRoundBytesShiftBE(storedData.data.get(), storedData.data.get() + storedData.length, shift);
            }
        }
        return *this;
    }

    // Method that checks the byte under the specified index.
    bool BinaryDataEngine::ByteStreamEngine::Test (const std::size_t index, const std::byte value) const noexcept
    {
        return (index < Length() && storedData.data[GetBytePosition(index)] == value);
    }

    // Method that returns byte sequence characteristic when all bytes have specified value in block of stored data.
    bool BinaryDataEngine::ByteStreamEngine::All (const std::byte value, std::size_t first, std::size_t last) const noexcept
    {
        if (last == npos) { last = Length() - 1; }
        if (first > last || last >= Length()) { return false; }

        while (first <= last)
        {
            if (Test(first++, value) == false) {
                return false;
            }
        }
        return true;
    }

    // Method that returns byte sequence characteristic when any of the bytes have specified value in block of stored data.
    bool BinaryDataEngine::ByteStreamEngine::Any (const std::byte value, std::size_t first, std::size_t last) const noexcept
    {
        if (last == npos) { last = Length() - 1; }
        if (first > last || last >= Length()) { return false; }

        while (first <= last)
        {
            if (Test(first++, value) == true) {
                return true;
            }
        }
        return false;
    }

    // Method that returns byte sequence characteristic when none of the bytes have a specified value in block of stored data.
    bool BinaryDataEngine::ByteStreamEngine::None (const std::byte value, std::size_t first, std::size_t last) const noexcept
    {
        if (last == npos) { last = Length() - 1; }
        if (first > last || last >= Length()) { return false; }

        while (first <= last)
        {
            if (Test(first++, value) == true) {
                return false;
            }
        }
        return true;
    }

    // Method that returns a pointer to the value of byte under the specified index.
    std::byte* BinaryDataEngine::ByteStreamEngine::GetAt (const std::size_t index) const noexcept
    {
        return (index < Length() ? &storedData.data[GetBytePosition(index)] : nullptr);
    }

}  // namespace types.
