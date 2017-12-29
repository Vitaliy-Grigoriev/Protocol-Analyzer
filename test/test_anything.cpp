// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>
#include <algorithm>

#include "../include/analyzer/AnalyzerApi.hpp"

using namespace analyzer;


int32_t main (int32_t size, char** data)
{
    /*uint8_t array[] = { 1, 2, 3, 4, 5 ,6, 7, 8, 9 };
    auto head = reinterpret_cast<std::byte*>(array);

    common::types::RawDataBuffer buffer = { };
    buffer.AssignData(array, array + sizeof(array));
    std::cout << buffer.Size() << std::endl;

    std::cout << buffer.BitsTransform() << std::endl;
    for (std::size_t idx = 0; idx < buffer.BitsTransform().Length(); ++idx) {
        buffer.BitsTransform().Invert(idx);
    }
    std::cout << buffer.BitsTransform() << std::endl;*/

    parser::PortsParser parse;
    parse.SetPorts("80;1-5;433;", ';');
    for (uint16_t port = parse.GetNextPort(); port != parser::PortsParser::end; port = parse.GetNextPort()) {
        std::cerr << "[+] Next port: " << port << std::endl;
    }

    std::cerr << "[+] Exit." << std::endl;
    return EXIT_SUCCESS;
}


