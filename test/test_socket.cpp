// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>

#include "../include/analyzer/Api.hpp"


static const auto CRLF_functor = [] (const char* data, std::size_t length) noexcept -> bool
{
    const char symbols[] = { 0x0D, 0x0A };
    return ( std::search(data, data + length, symbols, symbols + sizeof(symbols)) != data + length );
};

static const auto double_CRLF_functor = [] (const char* data, std::size_t length) noexcept -> bool
{
    const char symbols[] = { 0x0D, 0x0A, 0x0D, 0x0A };
    return ( std::search(data, data + length, symbols, symbols + sizeof(symbols)) != data + length );
};


int main(void)
{
    analyzer::net::Socket sock;
    if (sock.IsError()) {
        std::cout << "[error] Socket fail..." << std::endl;
        return EXIT_FAILURE;
    }

    sock.Bind(12345);
    if (sock.IsError()) {
        std::cout << "[error] Bind fail..." << std::endl;
        return EXIT_FAILURE;
    }

    sock.Connect("rucinema.net");
    if (sock.IsError()) {
        std::cout << "[error] Connection fail..." << std::endl;
        return EXIT_FAILURE;
    }

    char buff[] = "GET / HTTP/1.1\r\nHost: rucinema.net\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    sock.Send(buff, sizeof(buff));
    if (sock.IsError()) {
        std::cout << "[error] Send fail..." << std::endl;
        return EXIT_FAILURE;
    }

    char buff_receive[1000] = { };
    const int32_t len = sock.Recv(buff_receive, sizeof(buff_receive) - 1, double_CRLF_functor);
    if (sock.IsError()) {
        std::cout << "[error] Recv fail..." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Received data length: " << len << std::endl << std::endl << buff_receive << std::endl;

    sock.Close();
    return EXIT_SUCCESS;
}

