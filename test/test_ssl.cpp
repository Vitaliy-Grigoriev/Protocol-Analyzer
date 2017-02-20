// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>
#include "../include/analyzer/Api.hpp"


int main ()
{
    analyzer::net::SocketSSL sock;
    if (sock.IsError()) {
        std::cout << "Socket fail..." << std::endl;
        return EXIT_FAILURE;
    }

    if (!sock.SetHttp_1_1_OnlyProtocol()) {
        std::cout << "Set HTTP/1.1 only failed..." << std::endl;
        return EXIT_FAILURE;
    }
    if (!sock.SetOnlySecureCiphers()) {
        std::cout << "Secure ciphers failed..." << std::endl;
        return EXIT_FAILURE;
    }

    sock.Connect("habrahabr.ru");
    if (sock.IsError()) {
        std::cout << "Connection fail..." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Cipher name: " << sock.GetSelectedCipherName().c_str() << std::endl;

    char buff[] = "GET / HTTP/1.1\r\nHost: habrahabr.ru\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    sock.Send(buff, sizeof(buff));
    if (sock.IsError()) {
        std::cout << "Send fail..." << std::endl;
        return EXIT_FAILURE;
    }

    char buff_recv[DEFAULT_BUFFER_SIZE];
    int32_t len = sock.RecvToEnd(buff_recv, DEFAULT_BUFFER_SIZE);
    if (sock.IsError()) {
        std::cout << "Recv fail..." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Receiving data length: " << len << std::endl;
    analyzer::log::DbgHexDump("HTTP hex data of www.google.ru", buff_recv, std::size_t(len), 24);

    sock.Close();
    return EXIT_SUCCESS;
}
