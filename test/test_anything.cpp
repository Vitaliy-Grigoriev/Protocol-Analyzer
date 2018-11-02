// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#include <iostream>

#include "../include/framework/AnalyzerApi.hpp"

using namespace analyzer::framework;


int32_t main (int32_t size, char** data)
{
    log::Logger::Instance().SwitchLoggingEngine();
    log::Logger::Instance().SetLogLevel(log::LEVEL::INFORMATION);


    auto sock = system::allocMemoryForObject<net::Socket>(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock->Bind(12345) == false) {
        std::cout << "[error] Bind fail..." << std::endl;
        return EXIT_FAILURE;
    }

    const char buff[] = "111222333444\0";
    if (sock->SendTo("10.0.72.39", 138, buff, sizeof(buff)) == false) {
        std::cout << "[error] SendTo fail..." << std::endl;
        return EXIT_FAILURE;
    }

    sock->Close();


    std::cerr << "[+] Exit." << std::endl;
    return EXIT_SUCCESS;
}


