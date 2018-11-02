// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#include <net/if.h>  // IFF_*.
#include <unistd.h>  // getpid.
#include <net/if_arp.h>  // ARPHRD_*.
#include <linux/netlink.h>  // nlmsghdr.

#include "../../include/framework/System.hpp"  // system::allocMemoryForObject.
#include "../../include/framework/Common.hpp"  // common::GetRandomValue.
#include "../../include/framework/NetlinkInterface.hpp"


namespace analyzer::framework::net
{
    // Constructor of NetlinkSocket class.
    NetlinkSocket::NetlinkSocket (const uint32_t timeout) noexcept
        : Socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE, timeout)
    {
        pid = static_cast<uint32_t>((pthread_self() << 16) | static_cast<uint32_t>(getpid()));
    }

    // Method that associates a Netlink socket PID with a Netlink multicast groups.
    bool NetlinkSocket::Bind (const uint32_t groups) noexcept
    {
        sockaddr_nl netlink = { };
        netlink.nl_family = AF_NETLINK;
        netlink.nl_pid = pid;
        netlink.nl_groups = groups;

        const int32_t result = bind(fd, reinterpret_cast<sockaddr*>(&netlink), sizeof(struct sockaddr_nl));
        if (result == SOCKET_SUCCESS)
        {
            LOG_INFO("NetlinkSocket.Bind [", fd, "]: Binding to Netlink groups with pid '", pid, "' is success.");
            return true;
        }
        LOG_ERROR("NetlinkSocket.Bind [", fd, "]: Binding to  groups with pid '", pid, "' failed - ", GET_ERROR(errno));
        return false;
    }

    bool NetlinkReceiveFunctor (const char* const data, std::size_t length) noexcept
    {
        const struct nlmsghdr* msg = reinterpret_cast<const struct nlmsghdr*>(data);
        for ( ; NLMSG_OK(msg, length) == true; msg = NLMSG_NEXT(msg, length))
        {
            if (msg->nlmsg_type == NLMSG_DONE || msg->nlmsg_type == NLMSG_ERROR) {
                return true;
            }
        }
        return false;
    }

    InterfaceInfo* GetCorrectInterface (const uint32_t index, const uint8_t family, const std::list<InterfaceInfo>& interfaces)
    {
        for (auto iff = interfaces.begin(); iff != interfaces.end(); ++iff)
        {
            if (iff->interfaceIndex == index && (iff->interfaceFamily == AF_UNSPEC || iff->interfaceFamily == family))
            {
                return const_cast<InterfaceInfo*>(&*iff);
            }
        }
        return nullptr;
    }


    struct InterfaceRequest
    {
        struct nlmsghdr  netlinkHeaderMsg;
        struct ifinfomsg interfaceInfoMsg;
    };

    struct InterfaceAddressesRequest
    {
        struct nlmsghdr  netlinkHeaderMsg;
        struct ifaddrmsg interfaceAddressMsg;
    };

    struct InterfaceRoutesRequest
    {
        struct nlmsghdr netlinkHeaderMsg;
        struct rtmsg    interfaceRouteMsg;
    };


    bool NetlinkRequester::NetlinkInterfaceParser (void* const data, uint32_t length, std::list<InterfaceInfo>& interfaces) const noexcept
    {
        // Enumerate all received network interfaces.
        struct nlmsghdr* msg = static_cast<struct nlmsghdr*>(data);
        for ( ; NLMSG_OK(msg, length) == true; msg = NLMSG_NEXT(msg, length))
        {
            LOG_TRACE("NetlinkRequester.NetlinkInterfaceParser: Next Netlink message type: ", msg->nlmsg_type, '.');
            if (msg->nlmsg_type == RTM_NEWLINK)  // Get information about system interfaces.
            {
                struct ifinfomsg* const iface = static_cast<ifinfomsg*>(NLMSG_DATA(msg));
                uint32_t tailLength = NLMSG_PAYLOAD(msg, length);

                if (iface->ifi_type != ARPHRD_ETHER     &&
                    iface->ifi_type != ARPHRD_IEEE80211 &&
                    iface->ifi_type != ARPHRD_TUNNEL    &&
                    iface->ifi_type != ARPHRD_TUNNEL6   &&
                    iface->ifi_type != ARPHRD_LOOPBACK  &&
                    iface->ifi_type != 65534)
                {
                    LOG_INFO("NetlinkRequester.NetlinkInterfaceParser: Skip the following interface, type: ", iface->ifi_type, '.');
                    continue;
                }

                // Check active status of the interface.
                if (iface->ifi_flags & IFF_UP == 0u)
                {
                    LOG_INFO("NetlinkRequester.NetlinkInterfaceParser: Skip the following interface because its status is DOWN.");
                    continue;
                }
                
                InterfaceInfo interfaceEntry = { };
                interfaceEntry.interfaceFamily = iface->ifi_family;
                interfaceEntry.interfaceType = static_cast<uint32_t>(iface->ifi_type);
                interfaceEntry.interfaceIndex = static_cast<uint32_t>(iface->ifi_index);
                // Enumerate all attributes of each network interface.
                struct rtattr* attribute = IFLA_RTA(iface);
                for ( ; RTA_OK(attribute, tailLength) == true; attribute = RTA_NEXT(attribute, tailLength))
                {
                    const char* const attr = static_cast<const char*>(RTA_DATA(attribute));
                    switch (attribute->rta_type)
                    {
                        case IFLA_IFNAME:  // Interface name.
                            interfaceEntry.interfaceName = attr;
                            LOG_TRACE("NetlinkRequester.NetlinkInterfaceParser: Interface name: ", attr);
                            break;
                        case IFLA_ADDRESS:  // Interface MAC address.
                            interfaceEntry.macAddress = MacAddress(attr);
                            LOG_TRACE("NetlinkRequester.NetlinkInterfaceParser: Interface MAC address: ", interfaceEntry.macAddress.ToString());
                            break;
                        case IFLA_MTU:  // Interface MTU size.
                            interfaceEntry.mtuSize = *reinterpret_cast<const uint32_t*>(attr);
                            LOG_TRACE("NetlinkRequester.NetlinkInterfaceParser: Interface MTU size: ", interfaceEntry.mtuSize);
                            break;
                        default:
                            //LOG_INFO("Available additional information about interface, type ", attribute->rta_type, '.');
                            break;
                    }
                }
                interfaces.push_back(interfaceEntry);
            }
            else if (msg->nlmsg_type == NLMSG_DONE) { return true; }
            else if (msg->nlmsg_type == NLMSG_ERROR) { return false; }
            else { LOG_WARNING("NetlinkRequester.NetlinkInterfaceParser: Unhandled Netlink message, type: ", msg->nlmsg_type, '.'); }
        }
        return false;
    }


    bool NetlinkRequester::NetlinkAddressParser (void* const data, uint32_t length, std::list<InterfaceInfo>& interfaces) const noexcept
    {
        // Enumerate all received network interfaces address.
        struct nlmsghdr* msg = static_cast<struct nlmsghdr*>(data);
        for ( ; NLMSG_OK(msg, length) == true; msg = NLMSG_NEXT(msg, length))
        {
            LOG_TRACE("NetlinkRequester.NetlinkAddressParser: Next Netlink message type: ", msg->nlmsg_type, '.');
            if (msg->nlmsg_type == RTM_NEWADDR)  // Get information about system interfaces addresses.
            {
                struct ifaddrmsg* const addr = static_cast<ifaddrmsg*>(NLMSG_DATA(msg));
                uint32_t tailLength = NLMSG_PAYLOAD(msg, length);

                if (addr->ifa_family != AF_INET && addr->ifa_family != AF_INET6)
                {
                    LOG_INFO("NetlinkRequester.NetlinkAddressParser: Skip the following address, family: ", addr->ifa_family, '.');
                    continue;
                }

                InterfaceInfo* const interface = GetCorrectInterface(addr->ifa_index, addr->ifa_family, interfaces);
                if (interface == nullptr)
                {
                    LOG_WARNING("NetlinkRequester.NetlinkAddressParser: Not found correct interface, index: ", addr->ifa_index, ", family: ", addr->ifa_family, '.');
                    continue;
                }

                InterfaceAddresses ipAddressEntry = { };

                // Enumerate all attributes of each network interface address.
                struct rtattr* attribute = IFA_RTA(addr);
                for ( ; RTA_OK(attribute, tailLength) == true; attribute = RTA_NEXT(attribute, tailLength))
                {
                    const char* const attr = static_cast<const char*>(RTA_DATA(attribute));
                    switch (attribute->rta_type)
                    {
                        case IFA_ADDRESS:  // Interface address.
                            if (addr->ifa_family == AF_INET)
                                ipAddressEntry.ipAddress = IpAddress(*reinterpret_cast<const struct in_addr*>(attr));
                            else
                                ipAddressEntry.ipAddress = IpAddress(*reinterpret_cast<const struct in6_addr*>(attr));
                            LOG_TRACE("Interface address '", addr->ifa_index, "': ", ipAddressEntry.ipAddress.ToString());
                            break;
                        case IFA_LOCAL:  // Interface local address.
                            if (addr->ifa_family == AF_INET)
                                ipAddressEntry.localIpAddress = IpAddress(*reinterpret_cast<const struct in_addr*>(attr));
                            else
                                ipAddressEntry.localIpAddress = IpAddress(*reinterpret_cast<const struct in6_addr*>(attr));
                            LOG_TRACE("Interface local address '", addr->ifa_index, "': ", ipAddressEntry.localIpAddress.ToString());
                            break;
                        case IFA_BROADCAST:  // Interface broadcast address.
                            if (addr->ifa_family == AF_INET)
                                ipAddressEntry.broadcastIpAddress = IpAddress(*reinterpret_cast<const struct in_addr*>(attr));
                            else
                                ipAddressEntry.broadcastIpAddress = IpAddress(*reinterpret_cast<const struct in6_addr*>(attr));
                            LOG_TRACE("Interface broadcast address '", addr->ifa_index, "': ", ipAddressEntry.broadcastIpAddress.ToString());
                            break;
                        case IFA_ANYCAST:  // Interface anycast address.
                            if (addr->ifa_family == AF_INET)
                                ipAddressEntry.anycastIpAddress = IpAddress(*reinterpret_cast<const struct in_addr*>(attr));
                            else
                                ipAddressEntry.anycastIpAddress = IpAddress(*reinterpret_cast<const struct in6_addr*>(attr));
                            LOG_TRACE("Interface anycast address '", addr->ifa_index, "': ", ipAddressEntry.anycastIpAddress.ToString());
                            break;
                        default:
                            //LOG_INFO("Available additional information about interface addresses, type ", attribute->rta_type, '.');
                            break;
                    }
                }

                if (addr->ifa_family == AF_INET)
                    interface->ipv4Addresses.push_back(ipAddressEntry);
                else
                    interface->ipv6Addresses.push_back(ipAddressEntry);
            }
            else if (msg->nlmsg_type == NLMSG_DONE) { return true; }
            else if (msg->nlmsg_type == NLMSG_ERROR) { return false; }
            else { LOG_WARNING("NetlinkRequester.NetlinkAddressParser: Unhandled Netlink message, type: ", msg->nlmsg_type, '.'); }
        }
        return false;
    }

    bool NetlinkRequester::NetlinkRouteParser (void* const data, uint32_t length, std::list<InterfaceInfo>& interfaces) const noexcept
    {
        // Enumerate all received network interfaces route.
        struct nlmsghdr* msg = static_cast<struct nlmsghdr*>(data);
        for ( ; NLMSG_OK(msg, length) == true; msg = NLMSG_NEXT(msg, length))
        {
            LOG_TRACE("NetlinkRequester.NetlinkRouteParser: Next Netlink message type: ", msg->nlmsg_type, '.');
            if (msg->nlmsg_type == RTM_NEWROUTE)  // Get information about system interfaces routes.
            {
                struct rtmsg* const route = static_cast<rtmsg*>(NLMSG_DATA(msg));
                uint32_t tailLength = NLMSG_PAYLOAD(msg, length);

                if (route->rtm_family != AF_INET && route->rtm_family != AF_INET6)
                {
                    LOG_INFO("NetlinkRequester.NetlinkRouteParser: Skip the following route, family: ", uint16_t(route->rtm_family), '.');
                    continue;
                }
                if (route->rtm_type == RTN_BROADCAST || route->rtm_type == RTN_MULTICAST)
                {
                    LOG_INFO("NetlinkRequester.NetlinkRouteParser: Skip the following route, type: ", uint16_t(route->rtm_type), '.');
                    continue;
                }

                InterfaceRoutesInfo routeEntry = { };
                // Enumerate all attributes of each network interface route.
                struct rtattr* attribute = RTM_RTA(route);
                for ( ; RTA_OK(attribute, tailLength) == true; attribute = RTA_NEXT(attribute, tailLength))
                {
                    const char* const attr = static_cast<const char*>(RTA_DATA(attribute));
                    switch (attribute->rta_type)
                    {
                        case RTA_GATEWAY:  // Route gateway address.
                            if (route->rtm_family == AF_INET)
                                routeEntry.gateWay = IpAddress(*reinterpret_cast<const struct in_addr*>(attr));
                            else
                                routeEntry.gateWay = IpAddress(*reinterpret_cast<const struct in6_addr*>(attr));
                            LOG_TRACE("Route gateway address: ", routeEntry.gateWay.ToString());
                            break;
                        case RTA_DST:  // Route destination address.
                            if (route->rtm_family == AF_INET)
                                routeEntry.destinationAddress = IpAddress(*reinterpret_cast<const struct in_addr*>(attr));
                            else
                                routeEntry.destinationAddress = IpAddress(*reinterpret_cast<const struct in6_addr*>(attr));
                            LOG_TRACE("Route destination address: ", routeEntry.destinationAddress.ToString());
                            break;
                        case RTA_PREFSRC:  // Preferred route source address.
                            if (route->rtm_family == AF_INET)
                                routeEntry.sourceAddress = IpAddress(*reinterpret_cast<const struct in_addr*>(attr));
                            else
                                routeEntry.sourceAddress = IpAddress(*reinterpret_cast<const struct in6_addr*>(attr));
                            LOG_TRACE("Route source address: ", routeEntry.sourceAddress.ToString());
                            break;
                        case RTA_OIF:  // Interface index.
                            routeEntry.interfaceIndex = *reinterpret_cast<const uint32_t*>(attr);
                            LOG_TRACE("Route interface index: ", routeEntry.interfaceIndex);
                            break;
                        case RTA_PRIORITY:  // Route priority.
                            routeEntry.routePriority = *reinterpret_cast<const int32_t*>(attr);
                            LOG_TRACE("Route priority: ", routeEntry.routePriority);
                            break;
                        default:
                            //LOG_INFO("Available additional information about interface addresses, type ", attribute->rta_type, '.');
                            break;
                    }
                }
                
                InterfaceInfo* const interface = GetCorrectInterface(routeEntry.interfaceIndex, route->rtm_family, interfaces);
                if (interface == nullptr)
                {
                    LOG_WARNING("NetlinkRequester.NetlinkRouteParser: Not found correct interface, index: ", routeEntry.interfaceIndex, ", family: ", route->rtm_family, '.');
                    continue;
                }

                routeEntry.destinationMask.exist = true;
                if (route->rtm_family == AF_INET) {
                    routeEntry.destinationMask.isIPv6 = false;
                    if (route->rtm_dst_len == 0) {
                        routeEntry.destinationMask.ipv4.s_addr = 0x00000000;
                    }
                    else { routeEntry.destinationMask.ipv4.s_addr = htonl(static_cast<uint32_t>(~((1 << (32 - route->rtm_dst_len)) - 1))); }
                }
                else
                {
                    routeEntry.destinationMask.isIPv6 = true;
                    memset(&routeEntry.destinationMask.ipv6.s6_addr, 0xFF, sizeof(in6_addr));
                    for (uint16_t shift = 0; shift < 128 - route->rtm_dst_len; ++shift)
                    {
                        if (routeEntry.destinationMask.ipv6.s6_addr32[3] != 0) {
                            routeEntry.destinationMask.ipv6.s6_addr32[3] >>= 1;
                        }
                        else if (routeEntry.destinationMask.ipv6.s6_addr32[2] != 0) {
                            routeEntry.destinationMask.ipv6.s6_addr32[2] >>= 1;
                        }
                        else if (routeEntry.destinationMask.ipv6.s6_addr32[1] != 0) {
                            routeEntry.destinationMask.ipv6.s6_addr32[1] >>= 1;
                        }
                        else if (routeEntry.destinationMask.ipv6.s6_addr32[0] != 0) {
                            routeEntry.destinationMask.ipv6.s6_addr32[0] >>= 1;
                        }
                    }
                }

                if (routeEntry.sourceAddress.IsExist() == false)
                {
                    if (route->rtm_family == AF_INET) {
                        routeEntry.sourceAddress = interface->ipv4Addresses.front().ipAddress;
                    }
                    else {
                        routeEntry.sourceAddress = interface->ipv6Addresses.front().ipAddress;
                    }
                }
                routeEntry.routeScope = route->rtm_scope;
                routeEntry.interface = interface;
                routeEntry.isDefault = false;
                if (route->rtm_dst_len == 0 &&
                    routeEntry.routeScope == RT_SCOPE_UNIVERSE &&
                    routeEntry.gateWay.IsExist() == true &&
                    routeEntry.destinationAddress.IsExist() == false &&
                        route->rtm_family == AF_INET)
                {
                    routeEntry.isDefault = true;
                }

                if (route->rtm_family == AF_INET)
                {
                    interface->ipv4Routes.push_back(routeEntry);
                    if (routeEntry.isDefault == true) {
                        interface->defaultIpv4Route = &interface->ipv4Routes.back();
                    }
                }
                else { interface->ipv6Routes.push_back(routeEntry); }
            }
            else if (msg->nlmsg_type == NLMSG_DONE)
            {
                // Clear interfaces without routes.
                for (auto it = interfaces.cbegin(); it != interfaces.cend(); ++it)
                {
                    if (it->ipv4Routes.empty() == true && it->ipv6Routes.empty() == true) {
                        it = interfaces.erase(it);
                    }
                }
                return true;
            }
            else if (msg->nlmsg_type == NLMSG_ERROR) { return false; }
            else { LOG_WARNING("NetlinkRequester.NetlinkRouteParser: Unhandled Netlink message, type: ", msg->nlmsg_type, '.'); }
        }
        return false;
    }


    // Constructor of NetlinkRequester class.
    NetlinkRequester::NetlinkRequester (const uint8_t family) noexcept
        : interfaceFamily(family)
    {
        sock = system::allocMemoryForObject<NetlinkSocket>();
        if (sock == nullptr || sock->GetFd() == INVALID_SOCKET)
        {
            sock.reset(nullptr);
            LOG_ERROR("NetlinkRequester.NetlinkRequester: Error occurred when initialize Socket class.");
        }
    }

    // Method that returns the list of network interfaces of selected family in the system.
    bool NetlinkRequester::GetNetworkInterfaces (std::list<InterfaceInfo>& interfaces) noexcept
    {
        if (sock != nullptr)
        {
            InterfaceRequest request = { };

            request.netlinkHeaderMsg.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
            request.netlinkHeaderMsg.nlmsg_type = RTM_GETLINK;
            request.netlinkHeaderMsg.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
            request.netlinkHeaderMsg.nlmsg_seq = sequenceNumber++;
            request.netlinkHeaderMsg.nlmsg_pid = 0;

            request.interfaceInfoMsg.ifi_family = interfaceFamily;
            request.interfaceInfoMsg.ifi_change = 0xFFFFFFFF;

            LOG_ERROR("NetlinkRequester.GetNetworkInterfaces: Sending Netlink-Route request...");
            if (sock->Send(reinterpret_cast<char*>(&request), sizeof(InterfaceRequest)) == false) {
                return false;
            }

            char buff[16392] = { };
            std::size_t length = 0;
            const bool result = sock->Recv(buff, sizeof(buff), length, NetlinkReceiveFunctor);
            return result == true && length > 0 && NetlinkInterfaceParser(buff, static_cast<uint32_t>(length), interfaces) == true;
        }

        LOG_ERROR("NetlinkRequester.GetNetworkInterfaces: Socket is invalid.");
        return false;
    }

    // Method that fills the InterfaceInfo structures by network interface addresses.
    bool NetlinkRequester::GetInterfacesAddresses (std::list<InterfaceInfo>& interfaces) noexcept
    {
        if (interfaces.empty() == true) { return false; }

        if (sock != nullptr)
        {
            InterfaceAddressesRequest request = { };

            request.netlinkHeaderMsg.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
            request.netlinkHeaderMsg.nlmsg_type = RTM_GETADDR;
            request.netlinkHeaderMsg.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
            request.netlinkHeaderMsg.nlmsg_seq = sequenceNumber++;
            request.netlinkHeaderMsg.nlmsg_pid = 0;

            request.interfaceAddressMsg.ifa_family = interfaceFamily;

            LOG_ERROR("NetlinkRequester.GetInterfacesAddresses: Sending Netlink-Address request...");
            if (sock->Send(reinterpret_cast<char*>(&request), sizeof(InterfaceAddressesRequest)) == false) {
                return false;
            }

            char buff[16392] = { };
            std::size_t length = 0;
            const bool result = sock->Recv(buff, sizeof(buff), length, NetlinkReceiveFunctor);
            return result == true && length > 0 && NetlinkAddressParser(buff, static_cast<uint32_t>(length), interfaces) == true;
        }

        LOG_ERROR("NetlinkRequester.GetInterfacesAddresses: Socket is invalid.");
        return false;
    }


    bool NetlinkRequester::GetInterfacesRoutes (std::list<InterfaceInfo>& interfaces) noexcept
    {
        if (interfaces.empty() == true) { return false; }

        if (sock != nullptr)
        {
            InterfaceRoutesRequest request = { };

            request.netlinkHeaderMsg.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
            request.netlinkHeaderMsg.nlmsg_type = RTM_GETROUTE;
            request.netlinkHeaderMsg.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
            request.netlinkHeaderMsg.nlmsg_seq = sequenceNumber++;
            request.netlinkHeaderMsg.nlmsg_pid = 0;

            request.interfaceRouteMsg.rtm_family = interfaceFamily;

            LOG_ERROR("NetlinkRequester.GetInterfacesRoutes: Sending Netlink-Route request...");
            if (sock->Send(reinterpret_cast<char*>(&request), sizeof(InterfaceRoutesRequest)) == false) {
                return false;
            }

            char buff[16392] = { };
            std::size_t length = 0;
            const bool result = sock->Recv(buff, sizeof(buff), length, NetlinkReceiveFunctor);
            return result == true && length > 0 && NetlinkRouteParser(buff, static_cast<uint32_t>(length), interfaces) == true;
        }

        LOG_ERROR("NetlinkRequester.GetInterfacesRoutes: Socket is invalid.");
        return false;
    }

}  // namespace net.
