// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <map>
#include <iostream>

#include "../include/analyzer/Api.hpp"


int main(void)
{
    const std::string host = "habrahabr.ru";
    /*const auto protos = analyzer::utility::CheckSupportedTLSProtocols(host);
    if (protos.empty() == false)
    {
        std::cout << "Find next protocols on the host (" << host << "): " << std::endl;
        for (auto&& p : protos) {
            std::cout << p << std::endl;
        }
        std::cout << std::endl;
    }*/

    analyzer::net::SocketSSL sock(SSL_METHOD_TLS12, nullptr, DEFAULT_TIMEOUT_SSL);

    if (sock.SetHttp_1_1_OnlyProtocol() == false) {
        std::cout << "Set HTTP/1.1 only failed..." << std::endl;
        return EXIT_FAILURE;
    }
    if (sock.SetOnlySecureCiphers() == false) {
        std::cout << "Secure ciphers failed..." << std::endl;
        return EXIT_FAILURE;
    }

    if (sock.Connect(host.c_str()) == false) {
        std::cout << "Connection fail..." << std::endl;
        return EXIT_FAILURE;
    }

    //std::cout << "Session timeout: " << sock.GetSessionTimeout() << std::endl;
    std::cout << "Selected cipher name: " << sock.GetSelectedCipherName().c_str() << std::endl;

    char buff[] = "GET / HTTP/1.1\r\nHost: habrahabr.ru\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    const int32_t ret = sock.Send(buff, sizeof(buff));
    if (ret == -1) {
        std::cout << "Send fail..." << std::endl;
        return EXIT_FAILURE;
    }

    char buff_recv[DEFAULT_BUFFER_SIZE];
    const int32_t len = sock.RecvToEnd(buff_recv, DEFAULT_BUFFER_SIZE);
    if (len == -1) {
        std::cout << "Recv fail..." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Received data length: " << len << std::endl;

    //sock.Shutdown();
    sock.Close();
    return EXIT_SUCCESS;
}
