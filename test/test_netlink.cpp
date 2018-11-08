// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#include <iostream>

#include "../include/framework/AnalyzerApi.hpp"

using namespace analyzer::framework;


std::list<net::InterfaceInformation> interfaces;


const net::RouteInformation* GetBestRouteForIpAddress (const net::IpAddress& ip) noexcept
{
    if (interfaces.empty() == true) { return nullptr; }

    const net::RouteInformation* defaultRoute = interfaces.front().defaultIpv4Route;
    std::list<const net::RouteInformation*> routes;

    for (const auto& iff : interfaces)
    {
        if (iff.interfaceFamily == AF_UNSPEC || iff.interfaceFamily == AF_INET)
        {
            if (defaultRoute == nullptr) {
                defaultRoute = iff.defaultIpv4Route;
            }
            else if (iff.defaultIpv4Route != nullptr && iff.defaultIpv4Route->routePriority < defaultRoute->routePriority) {
                defaultRoute = iff.defaultIpv4Route;
            }

            for (auto rt = iff.ipv4Routes.cbegin(); rt != iff.ipv4Routes.cend(); ++rt)
            {
                if ((*rt)->isDefault == true) { continue; }

                if ((((*rt)->destinationAddress.ipv4.s_addr ^ ip.ipv4.s_addr) & (*rt)->destinationMask.ipv4.s_addr) == 0U)
                {
                    routes.push_back(&*(*rt));
                }
            }
        }
    }

    if (routes.empty() == true) {
        return defaultRoute;
    }
    if (routes.size() == 1) {
        return routes.front();
    }

    const net::RouteInformation* route = routes.front();
    for (auto rt = routes.cbegin(); rt != routes.cend(); ++rt)
    {
        if ((*rt)->routePriority < route->routePriority) {
            route = *rt;
        }
    }
    return route;
}


int32_t main (int32_t size, char** data)
{
    log::Logger::Instance().SwitchLoggingEngine();
    log::Logger::Instance().SetLogLevel(log::LEVEL::TRACE);
    log::Logger::Instance().SwitchBufferedMode();


    std::cout << storage::GI.GetNetworkInformation().ToString();

    /*net::NetlinkRequester netlink;
    const uint16_t type = net::INTERFACE_TYPE_ETHERNET;
    std::cout << netlink.GetNetworkInterfaces(interfaces, type) << std::endl;
    std::cout << netlink.GetInterfacesAddresses(interfaces, true) << std::endl;
    //std::cout << netlink.GetInterfacesRoutes(interfaces) << std::endl;
    for (auto& iff : interfaces) {
        std::cout << iff.ToString() << std::endl << std::endl;
    }*/

    /*net::IpAddress ip;
    ip.FromString(AF_INET, data[1]);
    std::cout << "IP address: " << ip.ToString() << std::endl;

    auto rt = GetBestRouteForIpAddress(ip);
    if (rt == nullptr) {
        std::cout << "Error!" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << rt->ToString() << std::endl;*/

    std::cerr << "[+] Exit." << std::endl;
    return EXIT_SUCCESS;
}
