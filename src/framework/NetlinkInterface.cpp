// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#include <net/if.h>  // IFF_*.
#include <unistd.h>  // getpid.
#include <pthread.h>  // pthread_self.
#include <net/if_arp.h>  // ARPHRD_*.
#include <linux/netlink.h>  // sockaddr_nl, nlmsghdr, NLMSG_*.

#include "../../include/framework/System.hpp"  // system::allocMemoryForObject.
#include "../../include/framework/Common.hpp"  // common::GetRandomValue.
#include "../../include/framework/NetlinkInterface.hpp"


namespace analyzer::framework::net
{
    // Constructor of NetlinkSocket class.
    NetlinkSocket::NetlinkSocket (const uint32_t timeout) noexcept
        : Socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE, timeout)
    {
        uniquePid = static_cast<uint32_t>((pthread_self() << 16) | static_cast<uint32_t>(getpid()));
    }

    // Method that associates a Netlink socket PID with a Netlink multicast groups.
    bool NetlinkSocket::Bind (const uint32_t groups) noexcept
    {
        sockaddr_nl netlink = { };
        netlink.nl_family = AF_NETLINK;
        netlink.nl_pid = uniquePid;
        netlink.nl_groups = groups;

        const int32_t result = bind(fd, reinterpret_cast<sockaddr*>(&netlink), sizeof(struct sockaddr_nl));
        if (result == SOCKET_SUCCESS)
        {
            LOG_INFO("NetlinkSocket.Bind [", fd, "]: Binding to Netlink groups with pid '", uniquePid, "' is success.");
            return true;
        }
        LOG_ERROR("NetlinkSocket.Bind [", fd, "]: Binding to  groups with pid '", uniquePid, "' failed - ", GET_ERROR(errno));
        return false;
    }

    // Default destructor of NetlinkSocket class.
    // Avoidance warning "-Wweak-vtables".
    NetlinkSocket::~NetlinkSocket(void) noexcept { }


    /**
     * @brief Functor that needs for fast receiving of the Netlink message.
     *
     * @param [in] data - Pointer to constant Netlink response message.
     * @param [in] length - Length of the Netlink message.
     * @return TRUE - if the Netlink-Done message was found in response, othewise - FALSE.
     */
    static bool NetlinkReceiveFunctor (const char* const data, std::size_t length) noexcept
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

    /**
     * @brief Function that returns the interface by index and network family.
     *
     * @param [in] index - Index of the network interface.
     * @param [in] family - Network family of the network interface.
     * @param [in] interfaces - List of the network interfaces in which the desired interface will be searched.
     * @return Pointer to the found network interface or nullptr.
     */
    [[nodiscard]]
    static InterfaceInformation* GetCorrectInterface (const uint32_t index, const uint8_t family, const std::list<InterfaceInformation>& interfaces) noexcept
    {
        for (auto iface = interfaces.cbegin(); iface != interfaces.cend(); ++iface)
        {
            if (iface->interfaceIndex == index)
            {
                if (family == AF_UNSPEC) {
                    return const_cast<InterfaceInformation*>(&*iface);
                }
                else if (iface->interfaceFamily == AF_UNSPEC || iface->interfaceFamily == family) {
                    return const_cast<InterfaceInformation*>(&*iface);
                }
            }
        }
        return nullptr;
    }

    // Method that parses Netlink-Interface response and fills InterfaceInformation structures.
    bool NetlinkRequester::NetlinkInterfaceParser (void* const data, uint32_t length, std::list<InterfaceInformation>& interfaces, const uint16_t types, const bool onlyRunning) const noexcept
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

                // Check type of the interface.
                if ((types & INTERFACE_TYPE_ANY) == 0U &&
                    !(iface->ifi_type == ARPHRD_ETHER && (types & INTERFACE_TYPE_ETHERNET) != 0U)      &&
                    !(iface->ifi_type == ARPHRD_IEEE80211 && (types & INTERFACE_TYPE_IEEE80211) != 0U) &&
                    !(iface->ifi_type == ARPHRD_TUNNEL && (types & INTERFACE_TYPE_TUNNEL) != 0U)       &&
                    !(iface->ifi_type == ARPHRD_TUNNEL6 && (types & INTERFACE_TYPE_TUNNEL6) != 0U)     &&
                    !(iface->ifi_type == ARPHRD_LOOPBACK && (types & INTERFACE_TYPE_LOOPBACK) != 0U)   &&
                    !(iface->ifi_type == 65534 && (types & INTERFACE_TYPE_UNSPECIFIED_TUNNEL) != 0U)   )
                {
                    LOG_INFO("NetlinkRequester.NetlinkInterfaceParser: Skip the following interface of type: ", iface->ifi_type, '.');
                    continue;
                }

                InterfaceInformation interfaceEntry = { };
                switch (iface->ifi_type)
                {
                    case ARPHRD_ETHER: interfaceEntry.interfaceType = INTERFACE_TYPE_ETHERNET;      break;
                    case ARPHRD_IEEE80211: interfaceEntry.interfaceType = INTERFACE_TYPE_IEEE80211; break;
                    case ARPHRD_TUNNEL: interfaceEntry.interfaceType = INTERFACE_TYPE_TUNNEL;       break;
                    case ARPHRD_TUNNEL6: interfaceEntry.interfaceType = INTERFACE_TYPE_TUNNEL6;     break;
                    case ARPHRD_LOOPBACK: interfaceEntry.interfaceType = INTERFACE_TYPE_LOOPBACK;   break;
                    case 65534: interfaceEntry.interfaceType = INTERFACE_TYPE_UNSPECIFIED_TUNNEL;   break;
                    default: interfaceEntry.interfaceType = iface->ifi_type;
                }

                // Check active status of the interface.
                if (onlyRunning == true && (iface->ifi_flags & IFF_UP) == 0U)
                {
                    LOG_INFO("NetlinkRequester.NetlinkInterfaceParser: Skip the following interface because its status is DOWN.");
                    continue;
                }

                interfaceEntry.interfaceFamily = iface->ifi_family;
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
                        case IFLA_BROADCAST:  // Interface broadcast MAC address.
                            interfaceEntry.broadcastMacAddress = MacAddress(attr);
                            LOG_TRACE("NetlinkRequester.NetlinkInterfaceParser: Interface broadcast MAC address: ", interfaceEntry.broadcastMacAddress.ToString());
                            break;
                        default:
                            LOG_INFO("NetlinkRequester.NetlinkInterfaceParser: Available additional interface information of type: ", attribute->rta_type, '.');
                            break;
                    }
                }
                interfaces.push_back(interfaceEntry);
            }
            else if (msg->nlmsg_type == NLMSG_DONE) { return true; }
            else if (msg->nlmsg_type == NLMSG_ERROR) { return false; }
            else { LOG_WARNING("NetlinkRequester.NetlinkInterfaceParser: Unhandled Netlink message of type: ", msg->nlmsg_type, '.'); }
        }
        return false;
    }

    // Method that parses Netlink-Address response and fills InterfaceInformation structures.
    bool NetlinkRequester::NetlinkAddressParser (void* const data, uint32_t length, std::list<InterfaceInformation>& addresses, const bool notEnrich) const noexcept
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
                    LOG_INFO("NetlinkRequester.NetlinkAddressParser: Skip the following address of family: ", addr->ifa_family, '.');
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
                        case IFA_ADDRESS:  // Interface unicast address.
                            if (addr->ifa_family == AF_INET)
                                ipAddressEntry.unicastIpAddress = IpAddress(*reinterpret_cast<const struct in_addr*>(attr));
                            else
                                ipAddressEntry.unicastIpAddress = IpAddress(*reinterpret_cast<const struct in6_addr*>(attr));
                            LOG_TRACE("NetlinkRequester.NetlinkAddressParser: Interface address '", addr->ifa_index, "': ", ipAddressEntry.unicastIpAddress.ToString());
                            break;
                        case IFA_LOCAL:  // Interface local address.
                            if (addr->ifa_family == AF_INET)
                                ipAddressEntry.localIpAddress = IpAddress(*reinterpret_cast<const struct in_addr*>(attr));
                            else
                                ipAddressEntry.localIpAddress = IpAddress(*reinterpret_cast<const struct in6_addr*>(attr));
                            LOG_TRACE("NetlinkRequester.NetlinkAddressParser: Interface local address '", addr->ifa_index, "': ", ipAddressEntry.localIpAddress.ToString());
                            break;
                        case IFA_ANYCAST:  // Interface anycast address.
                            if (addr->ifa_family == AF_INET)
                                ipAddressEntry.anycastIpAddress = IpAddress(*reinterpret_cast<const struct in_addr*>(attr));
                            else
                                ipAddressEntry.anycastIpAddress = IpAddress(*reinterpret_cast<const struct in6_addr*>(attr));
                            LOG_TRACE("NetlinkRequester.NetlinkAddressParser: Interface anycast address '", addr->ifa_index, "': ", ipAddressEntry.anycastIpAddress.ToString());
                            break;
                        case IFA_BROADCAST:  // Interface broadcast address.
                            if (addr->ifa_family == AF_INET)
                                ipAddressEntry.broadcastIpAddress = IpAddress(*reinterpret_cast<const struct in_addr*>(attr));
                            else
                                ipAddressEntry.broadcastIpAddress = IpAddress(*reinterpret_cast<const struct in6_addr*>(attr));
                            LOG_TRACE("NetlinkRequester.NetlinkAddressParser: Interface broadcast address '", addr->ifa_index, "': ", ipAddressEntry.broadcastIpAddress.ToString());
                            break;
                        case IFA_MULTICAST:  // Interface multicast address.
                            if (addr->ifa_family == AF_INET)
                                ipAddressEntry.multicastIpAddress = IpAddress(*reinterpret_cast<const struct in_addr*>(attr));
                            else
                                ipAddressEntry.multicastIpAddress = IpAddress(*reinterpret_cast<const struct in6_addr*>(attr));
                            LOG_TRACE("NetlinkRequester.NetlinkAddressParser: Interface multicast address '", addr->ifa_index, "': ", ipAddressEntry.multicastIpAddress.ToString());
                            break;
                        default:
                            LOG_INFO("NetlinkRequester.NetlinkAddressParser: Available additional addresses information of type ", attribute->rta_type, '.');
                            break;
                    }
                }

                const uint8_t family = (notEnrich == true ? addr->ifa_family : static_cast<uint8_t>(AF_UNSPEC));
                InterfaceInformation* const interface = GetCorrectInterface(addr->ifa_index, family, addresses);
                if (interface == nullptr && notEnrich == false)
                {
                    InterfaceInformation interfaceEntry = { };
                    interfaceEntry.interfaceIndex = addr->ifa_index;
                    interfaceEntry.interfaceFamily = addr->ifa_family;
                    if (addr->ifa_family == AF_INET) {
                        interfaceEntry.ipv4Addresses.push_back(ipAddressEntry);
                    }
                    else { interfaceEntry.ipv6Addresses.push_back(ipAddressEntry); }
                    addresses.push_back(interfaceEntry);
                }
                else if (interface != nullptr)
                {
                    if (addr->ifa_family == AF_INET) {
                        interface->ipv4Addresses.push_back(ipAddressEntry);
                    }
                    else { interface->ipv6Addresses.push_back(ipAddressEntry); }
                }
            }
            else if (msg->nlmsg_type == NLMSG_DONE) { return true; }
            else if (msg->nlmsg_type == NLMSG_ERROR) { return false; }
            else { LOG_WARNING("NetlinkRequester.NetlinkAddressParser: Unhandled Netlink message of type: ", msg->nlmsg_type, '.'); }
        }
        return false;
    }

    // Method that parses Netlink-Route response and fills RouteInformation structures.
    bool NetlinkRequester::NetlinkRouteParser (void* const data, uint32_t length, std::list<RouteInformation>& routes, const uint8_t types) const noexcept
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
                    LOG_INFO("NetlinkRequester.NetlinkRouteParser: Skip the following route of family: ", uint16_t(route->rtm_family), '.');
                    continue;
                }

                if (route->rtm_type == RTN_PROHIBIT || route->rtm_type == RTN_UNREACHABLE || route->rtm_type == RTN_BLACKHOLE)
                {
                    LOG_INFO("NetlinkRequester.NetlinkRouteParser: Skip the following route of type: ", uint16_t(route->rtm_type), '.');
                    continue;
                }

                if ((types & ROUTE_TYPE_ANY) == 0U &&
                    !(route->rtm_type == RTN_UNICAST && (types & ROUTE_TYPE_UNICAST) != 0U)     &&
                    !(route->rtm_type == RTN_BROADCAST && (types & ROUTE_TYPE_BROADCAST) != 0U) &&
                    !(route->rtm_type == RTN_MULTICAST && (types & ROUTE_TYPE_MULTICAST) != 0U) &&
                    !(route->rtm_type == RTN_ANYCAST && (types & ROUTE_TYPE_ANYCAST) != 0U)     &&
                    !(route->rtm_type == RTN_LOCAL && (types & ROUTE_TYPE_LOCAL) != 0U)         )
                {
                    LOG_INFO("NetlinkRequester.NetlinkRouteParser: Skip the following route of type: ", uint16_t(route->rtm_type), '.');
                    continue;
                }

                RouteInformation routeEntry = { };
                switch (route->rtm_type)
                {
                    case RTN_UNICAST: routeEntry.routeType = ROUTE_TYPE_UNICAST;     break;
                    case RTN_BROADCAST: routeEntry.routeType = ROUTE_TYPE_BROADCAST; break;
                    case RTN_MULTICAST: routeEntry.routeType = ROUTE_TYPE_MULTICAST; break;
                    case RTN_ANYCAST: routeEntry.routeType = ROUTE_TYPE_ANYCAST;     break;
                    case RTN_LOCAL: routeEntry.routeType = ROUTE_TYPE_LOCAL;         break;
                    default: routeEntry.routeType = route->rtm_type;
                }

                routeEntry.routeFamily = route->rtm_family;
                routeEntry.routeType = route->rtm_type;

                // Enumerate all attributes of each network interface route.
                struct rtattr* attribute = RTM_RTA(route);
                for ( ; RTA_OK(attribute, tailLength) == true; attribute = RTA_NEXT(attribute, tailLength))
                {
                    const char* const attr = static_cast<const char*>(RTA_DATA(attribute));
                    switch (attribute->rta_type)
                    {
                        case RTA_GATEWAY:  // Route gateway address.
                            if (route->rtm_family == AF_INET)
                                routeEntry.gatewayAddress = IpAddress(*reinterpret_cast<const struct in_addr*>(attr));
                            else
                                routeEntry.gatewayAddress = IpAddress(*reinterpret_cast<const struct in6_addr*>(attr));
                            LOG_TRACE("NetlinkRequester.NetlinkRouteParser: Route gateway address: ", routeEntry.gatewayAddress.ToString());
                            break;
                        case RTA_DST:  // Route destination address.
                            if (route->rtm_family == AF_INET)
                                routeEntry.destinationAddress = IpAddress(*reinterpret_cast<const struct in_addr*>(attr));
                            else
                                routeEntry.destinationAddress = IpAddress(*reinterpret_cast<const struct in6_addr*>(attr));
                            LOG_TRACE("NetlinkRequester.NetlinkRouteParser: Route destination address: ", routeEntry.destinationAddress.ToString());
                            break;
                        case RTA_PREFSRC:  // Preferred route source address.
                            if (route->rtm_family == AF_INET)
                                routeEntry.sourceAddress = IpAddress(*reinterpret_cast<const struct in_addr*>(attr));
                            else
                                routeEntry.sourceAddress = IpAddress(*reinterpret_cast<const struct in6_addr*>(attr));
                            LOG_TRACE("NetlinkRequester.NetlinkRouteParser: Route source address: ", routeEntry.sourceAddress.ToString());
                            break;
                        case RTA_OIF:  // Interface index.
                            routeEntry.interfaceIndex = *reinterpret_cast<const uint32_t*>(attr);
                            LOG_TRACE("NetlinkRequester.NetlinkRouteParser: Route interface index: ", routeEntry.interfaceIndex);
                            break;
                        case RTA_PRIORITY:  // Route priority.
                            routeEntry.routePriority = *reinterpret_cast<const int32_t*>(attr);
                            LOG_TRACE("NetlinkRequester.NetlinkRouteParser: Route priority: ", routeEntry.routePriority);
                            break;
                        default:
                            LOG_INFO("NetlinkRequester.NetlinkRouteParser: Available additional route information of type ", attribute->rta_type, '.');
                            break;
                    }
                }

                routeEntry.destinationMask.exist = true;
                if (route->rtm_family == AF_INET)
                {
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

                routeEntry.routeScope = route->rtm_scope;
                routeEntry.isDefault = false;
                if (route->rtm_dst_len == 0 &&
                    routeEntry.routeScope == RT_SCOPE_UNIVERSE &&
                    routeEntry.gatewayAddress.IsExist() == true &&
                    routeEntry.destinationAddress.IsExist() == false &&
                    route->rtm_family == AF_INET)
                {
                    routeEntry.isDefault = true;
                }
                routes.push_back(routeEntry);
            }
            else if (msg->nlmsg_type == NLMSG_DONE) { return true; }
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

    // Method that returns the list of network interfaces in the system.
    bool NetlinkRequester::GetNetworkInterfaces (std::list<InterfaceInformation>& interfaces, const uint16_t types, const bool onlyRunning) noexcept
    {
        if (sock != nullptr)
        {
            struct InterfaceRequest
            {
                struct nlmsghdr  netlinkHeaderMsg;
                struct ifinfomsg interfaceInfoMsg;
            } request = { };

            request.netlinkHeaderMsg.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
            request.netlinkHeaderMsg.nlmsg_type = RTM_GETLINK;
            request.netlinkHeaderMsg.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
            request.netlinkHeaderMsg.nlmsg_seq = sequenceNumber++;
            request.netlinkHeaderMsg.nlmsg_pid = 0;

            request.interfaceInfoMsg.ifi_family = interfaceFamily;
            request.interfaceInfoMsg.ifi_change = 0xFFFFFFFF;

            LOG_INFO("NetlinkRequester.GetNetworkInterfaces: Sending Netlink-Interface request...");
            if (sock->Send(reinterpret_cast<char*>(&request), sizeof(InterfaceRequest)) == false) {
                return false;
            }

            char buff[16392] = { };
            std::size_t length = 0;
            const bool result = sock->Recv(buff, sizeof(buff), length, NetlinkReceiveFunctor);
            return (result == true && length > 0 && NetlinkInterfaceParser(buff, static_cast<uint32_t>(length), interfaces, types, onlyRunning) == true);
        }

        LOG_ERROR("NetlinkRequester.GetNetworkInterfaces: Socket is invalid.");
        return false;
    }

    // Method that fills the InterfaceInfo structures by network interface addresses.
    bool NetlinkRequester::GetInterfacesAddresses (std::list<InterfaceInformation>& addresses, const bool notEnrich) noexcept
    {
        if (notEnrich == true && addresses.empty() == true)
        {
            LOG_WARNING("NetlinkRequester.GetInterfacesAddresses: Method call with incorrect inputted data.");
            return false;
        }

        if (sock != nullptr)
        {
            struct InterfaceAddressesRequest
            {
                struct nlmsghdr  netlinkHeaderMsg;
                struct ifaddrmsg interfaceAddressMsg;
            } request = { };

            request.netlinkHeaderMsg.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
            request.netlinkHeaderMsg.nlmsg_type = RTM_GETADDR;
            request.netlinkHeaderMsg.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
            request.netlinkHeaderMsg.nlmsg_seq = sequenceNumber++;
            request.netlinkHeaderMsg.nlmsg_pid = 0;

            request.interfaceAddressMsg.ifa_family = interfaceFamily;

            LOG_INFO("NetlinkRequester.GetInterfacesAddresses: Sending Netlink-Address request...");
            if (sock->Send(reinterpret_cast<char*>(&request), sizeof(InterfaceAddressesRequest)) == false) {
                return false;
            }

            char buff[16392] = { };
            std::size_t length = 0;
            const bool result = sock->Recv(buff, sizeof(buff), length, NetlinkReceiveFunctor);
            return (result == true && length > 0 && NetlinkAddressParser(buff, static_cast<uint32_t>(length), addresses, notEnrich) == true);
        }

        LOG_ERROR("NetlinkRequester.GetInterfacesAddresses: Socket is invalid.");
        return false;
    }

    // Method that fills the RouteInformation structures by network routes.
    bool NetlinkRequester::GetRoutes (std::list<RouteInformation>& routes, const uint8_t types) noexcept
    {
        if (sock != nullptr)
        {
            struct InterfaceRoutesRequest
            {
                struct nlmsghdr netlinkHeaderMsg;
                struct rtmsg    interfaceRouteMsg;
            } request = { };

            request.netlinkHeaderMsg.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
            request.netlinkHeaderMsg.nlmsg_type = RTM_GETROUTE;
            request.netlinkHeaderMsg.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
            request.netlinkHeaderMsg.nlmsg_seq = sequenceNumber++;
            request.netlinkHeaderMsg.nlmsg_pid = 0;

            request.interfaceRouteMsg.rtm_family = interfaceFamily;

            LOG_INFO("NetlinkRequester.GetRoutes: Sending Netlink-Route request...");
            if (sock->Send(reinterpret_cast<char*>(&request), sizeof(InterfaceRoutesRequest)) == false) {
                return false;
            }

            char buff[16392] = { };
            std::size_t length = 0;
            const bool result = sock->Recv(buff, sizeof(buff), length, NetlinkReceiveFunctor);
            return (result == true && length > 0 && NetlinkRouteParser(buff, static_cast<uint32_t>(length), routes, types) == true);
        }

        LOG_ERROR("NetlinkRequester.GetRoutes: Socket is invalid.");
        return false;
    }

}  // namespace net.
