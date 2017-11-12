// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>

#include "../include/analyzer/Api.hpp"


const auto CRLF_functor = [] (const char* data, std::size_t length) noexcept -> bool
{
    const char symbols[] = { 0x0D, 0x0A };
    return ( std::search(data, data + length, symbols, symbols + sizeof(symbols)) != data + length );
};

const auto double_CRLF_functor = [] (const char* data, std::size_t length) noexcept -> bool
{
    const char symbols[] = { 0x0D, 0x0A, 0x0D, 0x0A };
    return ( std::search(data, data + length, symbols, symbols + sizeof(symbols)) != data + length );
};

using namespace analyzer;


int32_t main (int32_t size, char** data)
{
    log::Logger::Instance().SwitchLoggingEngine();
    log::Logger::Instance().SetLogLevel(log::LEVEL::INFORMATION);

    auto sock = system::allocMemoryForObject<net::Socket>();

    if (sock->Bind(12345) == false) {
        std::cout << "[error] Bind fail..." << std::endl;
        return EXIT_FAILURE;
    }

    if (sock->Connect("rucinema.net") == false) {
        std::cout << "[error] Connection fail..." << std::endl;
        return EXIT_FAILURE;
    }

    const char buff[] = "GET / HTTP/1.1\r\nHost: rucinema.net\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    const int32_t ret = sock->Send(buff, sizeof(buff));
    if (ret == -1) {
        std::cout << "[error] Send fail..." << std::endl;
        return EXIT_FAILURE;
    }

    int32_t length = 0;
    char buff_receive[1024] = { };
    const bool result = sock->Recv(buff_receive, sizeof(buff_receive) - 1, length, double_CRLF_functor, 250);
    if (result == false) {
        std::cout << "[error] Recv fail..." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Received data length: " << length << std::endl << std::endl << buff_receive << std::endl;

    sock->Close();
    return EXIT_SUCCESS;
}

