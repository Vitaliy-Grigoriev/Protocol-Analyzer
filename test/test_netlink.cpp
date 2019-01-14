// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#include <iostream>

#include "FrameworkApi.hpp"

using namespace analyzer::framework;


int32_t main (int32_t size, char** data)
{
    log::Logger::Instance().SwitchLoggingEngine();
    log::Logger::Instance().SetLogLevel(log::LEVEL::TRACE);
    log::Logger::Instance().SwitchBufferedMode();


    std::cout << storage::GI.GetNetworkInformation().ToString();
    

    net::IpAddress ip = { };
    ip.FromString(AF_INET, data[1]);
    std::cout << "Selected IP address: " << ip.ToString() << std::endl;

    auto rt = storage::GI.GetNetworkInformation().GetBestRouteForIpAddress(ip);
    if (rt == nullptr) {
        std::cout << "Error!" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << rt->ToString() << std::endl;

    std::cerr << "[+] Exit." << std::endl;
    return EXIT_SUCCESS;
}
