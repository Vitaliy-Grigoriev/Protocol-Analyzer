// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#include "../../include/framework/NetlinkInterface.hpp"  // net::NetlinkRequester.
#include "../../include/framework/SystemNetworkConfiguration.hpp"


namespace analyzer::framework::system
{
    // Method that initialize information about system network interfaces and routes.
    bool SystemNetworkConfiguration::Initialize (const uint8_t family) noexcept
    {
        if (family != AF_INET && family != AF_INET6 && family != AF_UNSPEC)
        {
            LOG_FATAL("SystemNetworkConfiguration.Initialize: Incorrect type of inputted interface network family.");
            return false;
        }

        interfaceFamily = family;
        net::NetlinkRequester netlink(family);
        if (netlink.GetNetworkInterfaces(networkInterfacesInfo) == false)
        {
            LOG_FATAL("SystemNetworkConfiguration.Initialize: An error occurred while retrieving network interfaces information.");
            return false;
        }

        if (netlink.GetInterfacesAddresses(networkInterfacesInfo, true) == false)
        {
            LOG_FATAL("SystemNetworkConfiguration.Initialize: An error occurred while retrieving network addresses information.");
            return false;
        }

        if (netlink.GetRoutes(networkRoutesInfo) == false)
        {
            LOG_FATAL("SystemNetworkConfiguration.Initialize: An error occurred while retrieving network routes information.");
            return false;
        }

        for (auto route = networkRoutesInfo.begin(); route != networkRoutesInfo.end(); ++route)
        {
            net::InterfaceInformation* const interface = const_cast<net::InterfaceInformation*>(GetInterface(route->interfaceIndex, route->routeFamily));
            if (interface == nullptr)
            {
                LOG_TRACE("SystemNetworkConfiguration.Initialize: Not found correct interface by index: ", route->interfaceIndex, ", family: ", route->routeFamily, '.');
                route = networkRoutesInfo.erase(route);
                continue;
            }

            route->ownerInterface = interface;
            if (route->routeFamily == AF_INET)
            {
                interface->ipv4Routes.push_back(&*route);
                if (route->isDefault == true) {
                    interface->defaultIpv4Route = &*route;
                }
            }
            else { interface->ipv6Routes.push_back(&*route); }

            if (route->sourceAddress.IsExist() == false)
            {
                if (route->routeFamily == AF_INET) {
                    route->sourceAddress = interface->ipv4Addresses.front().unicastIpAddress;
                }
                else {
                    route->sourceAddress = interface->ipv6Addresses.front().unicastIpAddress;
                }
            }
        }

        // Clear interfaces without routes.
        for (auto it = networkInterfacesInfo.cbegin(); it != networkInterfacesInfo.cend(); ++it)
        {
            if (it->ipv4Routes.empty() == true && it->ipv6Routes.empty() == true) {
                it = networkInterfacesInfo.erase(it);
            }
        }
        return true;
    }

    // Method that updates information about system network interfaces and routes.
    bool SystemNetworkConfiguration::Update (const uint8_t family) noexcept
    {
        networkRoutesInfo.clear();
        networkInterfacesInfo.clear();
        return Initialize(family);
    }

    // Method that returns the best network route for selected IP address.
    const net::RouteInformation* SystemNetworkConfiguration::GetBestRouteForIpAddress (const net::IpAddress& ip) noexcept
    {
        if (networkInterfacesInfo.empty() == true || ip.IsExist() == false) { return nullptr; }
        try { std::lock_guard<std::mutex> lock { mutex }; }
        catch (const std::system_error& /*err*/) {
            return nullptr;
        }

        const net::RouteInformation* defaultRoute = networkInterfacesInfo.front().defaultIpv4Route;
        std::list<const net::RouteInformation*> routes;  // List of found candidates for routing.

        bool alreadyUpdate = false;
        while (true)
        {
            for (const auto& iface : networkInterfacesInfo)
            {
                if (ip.isIPv6 == false && (iface.interfaceFamily == AF_UNSPEC || iface.interfaceFamily == AF_INET))
                {
                    if (defaultRoute == nullptr) {
                        defaultRoute = iface.defaultIpv4Route;
                    }
                    else if (iface.defaultIpv4Route != nullptr && iface.defaultIpv4Route->routePriority < defaultRoute->routePriority) {
                        defaultRoute = iface.defaultIpv4Route;
                    }

                    for (auto ipv4Route : iface.ipv4Routes) {
                        if (ipv4Route->isDefault == true) { continue; }
                        if (((ipv4Route->destinationAddress.ipv4.s_addr ^ ip.ipv4.s_addr) & ipv4Route->destinationMask.ipv4.s_addr) == 0U)
                        {
                            routes.push_back(&*ipv4Route);
                        }
                    }
                }
                else
                {
                    // IPv6 not implemented now.
                    return nullptr;
                }
            }

            // Check condition for exit from loop.
            if (routes.empty() == true && defaultRoute == nullptr)
            {
                if (alreadyUpdate == true || Update() == false) {
                    return nullptr;
                }
                alreadyUpdate = true;
            }
            else { break; }  // Route found.
        }

        if (routes.empty() == true) {
            return defaultRoute;
        }
        if (routes.size() == 1) {
            return routes.front();
        }

        const net::RouteInformation* route = routes.front();
        for (const auto rt : routes)
        {
            if (rt->routePriority < route->routePriority) {
                route = rt;
            }
        }
        return route;
    }

    // Method that returns the interface by index and network family.
    const net::InterfaceInformation* SystemNetworkConfiguration::GetInterface (const uint32_t index, const uint8_t family) noexcept
    {
        for (const auto& iface : networkInterfacesInfo) {
            if (iface.interfaceIndex == index)
            {
                if (family == AF_UNSPEC) {
                    return const_cast<net::InterfaceInformation*>(&iface);
                }
                else if (iface.interfaceFamily == AF_UNSPEC || iface.interfaceFamily == family) {
                    return const_cast<net::InterfaceInformation*>(&iface);
                }
            }
        }
        return nullptr;
    }

    // Method that returns information about system interfaces and routes in string format.
    std::string SystemNetworkConfiguration::ToString(void) const noexcept
    {
        std::ostringstream str;
        for (auto& iface : networkInterfacesInfo) {
            str << iface.ToString() << std::endl << std::endl;
        }
        return str.str();
    }

}  // namespace system.
