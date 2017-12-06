// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <mutex>
#include <utility>
#include <iostream>
#include <algorithm>

#include "../include/analyzer/AnalyzerApi.hpp"

using namespace analyzer;


void leftRoundBytesShift (std::byte* head, const std::byte* end, std::byte* newHead) noexcept
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

void leftRoundBytesShift (std::byte* head, const std::byte* end, const std::size_t shift) noexcept
{
    leftRoundBytesShift(head, end, head + shift);
}


void rightRoundBytesShift (const std::byte* head, std::byte* end, std::byte* newEnd) noexcept
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

void rightRoundBytesShift (const std::byte* head, std::byte* end, const std::size_t shift) noexcept
{
    rightRoundBytesShift(head, end, end - shift - 1);
}


void leftDirectBytesShift (std::byte* head, const std::byte* end, std::byte* newHead, bool value = false) noexcept
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
    memset(head, (value == false ? 0x00 : 0xFF), static_cast<std::size_t>(end - head));
}

void leftDirectBytesShift (std::byte* head, const std::byte* end, const std::size_t shift, bool value = false) noexcept
{
    leftDirectBytesShift(head, end, head + shift, value);
}


void rightDirectBytesShift (std::byte* head, std::byte* end, std::byte* newEnd, bool value = false) noexcept
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
    memset(head, (value == false ? 0x00 : 0xFF), static_cast<std::size_t>(end - prev));
}

void rightDirectBytesShift (std::byte* head, std::byte* end, const std::size_t shift, bool value = false) noexcept
{
    rightDirectBytesShift(head, end, end - shift - 1, value);
}


void leftBitsShift (std::byte* data, const std::size_t size, std::size_t shift, bool isRound) noexcept
{
    if (data != nullptr && size > 0)
    {
        if (shift >= size) {
            if (isRound == true) { shift %= size; }
            else {
                memset(data, 0, size);
                return;
            }
        }

        if (size > 1)
        {
            const std::size_t numberOfIterations = shift / sizeof(std::byte);
            if (numberOfIterations > 0)
            {
                const std::size_t tail = shift % sizeof(std::byte);
                if (isRound == false) {
                    for (std::size_t jdx = 0; jdx < size - 1; ++jdx) {
                        data[jdx] = data[jdx + 1];
                    }
                }
                else
                {
                    std::rotate(data, data + numberOfIterations, data + size);
                    const std::byte head = data[0];
                    data[size - 1] = head;
                }
            }
            else
            {
                const std::byte head = data[0];
                for (std::size_t idx = 0; idx < size - 1; ++idx) {
                    data[idx] = (data[idx + 1] >> (sizeof(std::byte) - shift)) | (data[idx] << shift);
                }
                if (isRound == false) {
                    data[size - 1] <<= shift;
                }
                else { data[size - 1] = (head >> (sizeof(std::byte) - shift)) | (data[size - 1] << shift); }
            }
        }
        else {
            if (isRound == true) {
                *data = (*data >> (sizeof(std::byte) - shift)) | (*data << shift);
            }
            else { *data <<= shift; }
        }
    }
}

int32_t main (int32_t size, char** data)
{
    uint8_t array[] = { 1, 2, 3, 4, 5 ,6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
    auto head = reinterpret_cast<std::byte*>(array);



    return EXIT_SUCCESS;

    std::string text = "0123456789";
    common::types::RawDataBuffer buffer1 = { };
    buffer1.AssignData(text.data(), text.size());
    std::cout << buffer1.Size() << std::endl;

    common::types::RawDataBuffer buffer2(buffer1);
    std::cout << buffer1.Size() << std::endl;
    std::cout << buffer2.Size() << std::endl;

    common::types::RawDataBuffer buffer3 = std::move(buffer1);
    std::cout << buffer1.Size() << std::endl;
    std::cout << buffer3.Size() << std::endl;

    std::cout << char(buffer3[5]) << std::endl;
    std::cout << char(buffer3[20]) << std::endl;

    std::cerr << "[+] Exit." << std::endl;
    return EXIT_SUCCESS;
}


