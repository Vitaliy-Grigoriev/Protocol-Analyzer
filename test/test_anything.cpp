// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <mutex>
#include <utility>
#include <iostream>
#include <algorithm>

#include "../include/analyzer/AnalyzerApi.hpp"

using namespace analyzer;


int32_t main (int32_t size, char** data)
{
    uint8_t array[] = { 1, 2, 3, 4, 5 ,6, 7, 8, 9 };
    auto head = reinterpret_cast<std::byte*>(array);

    common::types::RawDataBuffer buffer = { };
    buffer.AssignData(array, array + sizeof(array));
    std::cout << buffer.Size() << std::endl;

    std::cout << buffer.BitsTransform() << std::endl;
    for (std::size_t idx = 0; idx < buffer.BitsTransform().Length(); ++idx) {
        buffer.BitsTransform().Invert(idx);
    }
    std::cout << buffer.BitsTransform() << std::endl;



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


