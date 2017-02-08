// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>
#include "../include/Api.hpp"


int main ()
{
    analyzer::net::SocketSSL sock;
    if (sock.IsError()) {
        std::cout << "Socket fail..." << std::endl;
        return EXIT_FAILURE;
    }

    if (!sock.SetHttpProtocols()) {
        std::cout << "Set all HTTP protocols failed..." << std::endl;
        return EXIT_FAILURE;
    }
    if (!sock.SetOnlySecureCiphers()) {
        std::cout << "Secure ciphers failed..." << std::endl;
        return EXIT_FAILURE;
    }

    sock.Connect("www.google.ru");
    if (sock.IsError()) {
        std::cout << "Connection fail..." << std::endl;
        return EXIT_FAILURE;
    }

    switch (sock.GetSelectedHttpProtocols())
    {
        case HTTP_VERSION::HTTP1_1:
            std::cout << "HTTP/1.1 protocol." << std::endl;
            break;
        case HTTP_VERSION::HTTP2_0:
            std::cout << "HTTP/2.0 protocol." << std::endl;
            break;
        case HTTP_VERSION::ERROR:
            std::cout << "ALPN protocol ERROR." << std::endl;
            break;
    }

    sock.Close();
    return EXIT_SUCCESS;
}


