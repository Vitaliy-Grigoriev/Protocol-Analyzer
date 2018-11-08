// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#include "../../include/framework/NetlinkInterface.hpp"  // net::NetlinkRequester.
#include "../../include/framework/SystemNetworkConfiguration.hpp"


namespace analyzer::framework::system
{
    // Method that initialize information about system network interfaces and routes.
    bool SystemNetworkConfiguration::Initialize (const uint8_t family) noexcept
    {
        net::NetlinkRequester netlink(family);
        if (netlink.GetNetworkInterfaces(networkInterfacesInfo) == false)
        {
            LOG_FATAL("SystemNetworkConfiguration.SystemNetworkConfiguration: An error occurred while retrieving network interface information.");
            return false;
        }

        if (netlink.GetInterfacesAddresses(networkInterfacesInfo, true) == false)
        {
            LOG_FATAL("SystemNetworkConfiguration.SystemNetworkConfiguration: An error occurred while retrieving network interfaces addresses.");
            return false;
        }

        if (netlink.GetRoutes(networkRoutesInfo) == false)
        {
            LOG_FATAL("SystemNetworkConfiguration.SystemNetworkConfiguration: An error occurred while retrieving network routes.");
            return false;
        }

        for (auto route = networkRoutesInfo.begin(); route != networkRoutesInfo.end(); ++route)
        {
            net::InterfaceInformation* const interface = GetCorrectInterface(route->interfaceIndex, route->routeFamily, networkInterfacesInfo);
            if (interface == nullptr)
            {
                LOG_TRACE("SystemNetworkConfiguration.SystemNetworkConfiguration: Not found correct interface with index: ", route->interfaceIndex, ", family: ", route->routeFamily, '.');
                route = networkRoutesInfo.erase(route);
                continue;
            }

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

    std::string SystemNetworkConfiguration::ToString (void) const noexcept
    {
        std::ostringstream str;
        for (auto& iface : networkInterfacesInfo) {
            str << iface.ToString() << std::endl << std::endl;
        }
        return str.str();
    }


}  // namespace system.
