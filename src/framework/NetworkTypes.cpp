// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#include <sstream>  // std::string, std::ostringstream.

#include "../../include/framework/NetworkTypes.hpp"


namespace analyzer::framework::net
{
    /*********************************************** IpAddress ***********************************************/

    // Copy assignment constructor of IpAddress class.
    IpAddress::IpAddress (const IpAddress& other) noexcept
    {
        if (other.exist == true)
        {
            exist = true;
            isIPv6 = other.isIPv6;
            if (isIPv6 == true) {
                memcpy(ipv6.s6_addr, other.ipv6.s6_addr, sizeof(struct in6_addr));
                return;
            }
            memcpy(&ipv4.s_addr, &other.ipv4.s_addr, sizeof(struct in_addr));
        }
    }

    // Move assignment constructor of IpAddress class.
    IpAddress::IpAddress (IpAddress&& other) noexcept
    {
        if (other.exist == true)
        {
            exist = true;
            isIPv6 = other.isIPv6;
            if (isIPv6 == true)
            {
                memcpy(ipv6.s6_addr, other.ipv6.s6_addr, sizeof(struct in6_addr));
                memset(other.ipv6.s6_addr, 0x00, sizeof(struct in6_addr));
                return;
            }
            memcpy(&ipv4.s_addr, &other.ipv4.s_addr, sizeof(struct in_addr));
            memset(&other.ipv4.s_addr, 0x00, sizeof(struct in_addr));
        }
    }

    // Copy assignment operator of IpAddress class.
    IpAddress& IpAddress::operator= (const IpAddress& other) noexcept
    {
        if (this != &other && other.exist == true)
        {
            exist = true;
            isIPv6 = other.isIPv6;
            if (isIPv6 == true) {
                memcpy(ipv6.s6_addr, other.ipv6.s6_addr, sizeof(struct in6_addr));
                return *this;
            }
            memcpy(&ipv4.s_addr, &other.ipv4.s_addr, sizeof(struct in_addr));
        }
        return *this;
    }

    // Move assignment operator of IpAddress class.
    IpAddress& IpAddress::operator= (IpAddress&& other) noexcept
    {
        if (this != &other && other.exist == true)
        {
            exist = true;
            isIPv6 = other.isIPv6;
            if (isIPv6 == true)
            {
                memcpy(ipv6.s6_addr, other.ipv6.s6_addr, sizeof(struct in6_addr));
                memset(other.ipv6.s6_addr, 0x00, sizeof(struct in6_addr));
                return *this;
            }
            memcpy(&ipv4.s_addr, &other.ipv4.s_addr, sizeof(struct in_addr));
            memset(&other.ipv4.s_addr, 0x00, sizeof(struct in_addr));
        }
        return *this;
    }

    // Method that return the result of comparison of the two IP addresses.
    bool IpAddress::operator< (const IpAddress& other) const noexcept
    {
        if (isIPv6 != other.isIPv6) {
            return !isIPv6;
        }

        if (isIPv6 == true) {
            return (memcmp(ipv6.s6_addr, other.ipv6.s6_addr, sizeof(ipv6)) < 0);
        }
        return (ipv4.s_addr < other.ipv4.s_addr);
    }

    // Method that return the result of comparison of the two IP addresses.
    bool IpAddress::operator> (const IpAddress& other) const noexcept
    {
        return !(*this < other) && !(*this == other);
    }

    // Method that return the result of comparison of the two IP addresses.
    bool IpAddress::operator== (const IpAddress& other) const noexcept
    {
        if (isIPv6 != other.isIPv6) {
            return false;
        }

        if (isIPv6 == true) {
            return (memcmp(ipv6.s6_addr, other.ipv6.s6_addr, sizeof(ipv6)) == 0);
        }
        return (ipv4.s_addr == other.ipv4.s_addr);
    }

    // Method that returns stored IP address in 'sockaddr_storage' representation.
    struct sockaddr_storage IpAddress::GetSockaddr(void) const noexcept
    {
        if (isIPv6 == true)
        {
            sockaddr_in6 sock = { };
            sock.sin6_family = AF_INET6;
            sock.sin6_addr = ipv6;
            return *reinterpret_cast<sockaddr_storage*>(&sock);
        }

        sockaddr_in sock = { };
        sock.sin_family = AF_INET;
        sock.sin_addr = ipv4;
        return *reinterpret_cast<sockaddr_storage*>(&sock);
    }

    // Method that returns IP address in string format.
    std::string IpAddress::ToString(void) const noexcept
    {
        if (isIPv6 == true)
        {
            char str[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, static_cast<const void*>(&ipv6), str, INET6_ADDRSTRLEN);
            return str;
        }
        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, static_cast<const void*>(&ipv4), str, INET_ADDRSTRLEN);
        return str;
    }

    // Method that assigns IP address from string format.
    bool IpAddress::FromString (const uint16_t family, const std::string& ip) noexcept
    {
        exist = true;
        if (family == AF_INET) {
            isIPv6 = false;
            return (inet_pton(AF_INET, ip.c_str(), &ipv4) == 1);
        }
        isIPv6 = true;
        return (inet_pton(AF_INET6, ip.c_str(), &ipv6) == 1);
    }

    /*********************************************** IpAddress ***********************************************/


    /*********************************************** MacAddress ***********************************************/

    // Constructor that initializes MAC address.
    MacAddress::MacAddress (const char addr[6]) noexcept
    {
        memcpy(&address, addr, sizeof(address));
    }

    // Copy assignment constructor of MacAddress class.
    MacAddress::MacAddress (const MacAddress& other) noexcept
    {
        memcpy(address, other.address, sizeof(address));
    }

    // Move assignment constructor of MacAddress class.
    MacAddress::MacAddress (MacAddress&& other) noexcept
    {
        memcpy(address, other.address, sizeof(address));
        memset(other.address, 0x00, sizeof(other.address));
    }

    // Copy assignment operator of MacAddress class.
    MacAddress& MacAddress::operator= (const MacAddress& other) noexcept
    {
        if (this != &other) {
            memcpy(address, other.address, sizeof(address));
        }
        return *this;
    }

    //  Move assignment operator of MacAddress class.
    MacAddress& MacAddress::operator= (MacAddress&& other) noexcept
    {
        if (this != &other) {
            memcpy(address, other.address, sizeof(address));
            memset(other.address, 0x00, sizeof(other.address));
        }
        return *this;
    }

    // Method that returns MAC address in string format.
    std::string MacAddress::ToString(void) const noexcept
    {
        char out[18] = { };
        snprintf(out, 18, "%02x:%02x:%02x:%02x:%02x:%02x", address[0], address[1], address[2], address[3], address[4], address[5]);
        return std::string(out);
    }

    // Method that assigns MAC address from string format.
    bool MacAddress::FromString (const std::string& mac) noexcept
    {
        uint16_t tokenCount = 0;
        char* ctx = nullptr;
        char* token = strtok_r(const_cast<char*>(mac.c_str()), "-:", &ctx);

        while (token != nullptr)
        {
            const uint64_t val = static_cast<uint64_t>(std::strtoul(token, nullptr, 16));
            if (val > 0xFF) { return false; }

            address[tokenCount++] = static_cast<uint8_t>(val);
            token = strtok_r(nullptr, "-:", &ctx);
        }
        return (tokenCount == 6);
    }

    /*********************************************** MacAddress ***********************************************/


    /******************************************* InterfaceAddresses *******************************************/

    std::string InterfaceAddresses::ToString(void) const noexcept
    {
        std::ostringstream str;
        str << "\tIP unicast address:   " << unicastIpAddress   << std::endl;
        str << "\tIP local address:     " << localIpAddress     << std::endl;
        str << "\tIP anycast address:   " << anycastIpAddress   << std::endl;
        str << "\tIP broadcast address: " << broadcastIpAddress << std::endl;
        str << "\tIP multicast address: " << multicastIpAddress << std::endl;
        return str.str();
    }

    /******************************************* InterfaceAddresses *******************************************/


    /******************************************** RouteInformation ********************************************/

    std::string RouteInformation::ToString(void) const noexcept
    {
        std::ostringstream str;
        str << "\tSource address:      " << sourceAddress         << std::endl;
        str << "\tDestination address: " << destinationAddress    << std::endl;
        str << "\tDestination mask:    " << destinationMask       << std::endl;
        str << "\tGateWay address:     " << gatewayAddress        << std::endl;
        str << "\tIs default:          " << isDefault             << std::endl;
        str << "\tPriority:            " << routePriority         << std::endl;
        str << "\tFamily:              " << uint16_t(routeFamily) << std::endl;
        str << "\tType:                " << uint16_t(routeType)   << std::endl;
        return str.str();
    }

    /******************************************** RouteInformation ********************************************/


    /****************************************** InterfaceInformation ******************************************/

    std::string InterfaceInformation::ToString(void) const noexcept
    {
        std::ostringstream str;
        str << "Interface name:   " << interfaceName                  << std::endl;
        str << "Interface index:  " << interfaceIndex                 << std::endl;
        str << "Interface type:   " << interfaceType                  << std::endl;
        str << "MTU size:         " << mtuSize                        << std::endl;
        str << "Interface family: " << uint16_t(interfaceFamily)      << std::endl;
        str << "Interface MAC:    " << macAddress.ToString()          << std::endl;
        str << "Broadcast MAC:    " << broadcastMacAddress.ToString() << std::endl;

        str << "IPv4 addresses: " << std::endl;
        for (auto& address : ipv4Addresses) {
            str << address.ToString() << std::endl;
        }
        str << "IPv6 addresses: " << std::endl;
        for (auto& address6 : ipv6Addresses) {
            str << address6.ToString() << std::endl;
        }

        str << "IPv4 routes: " << std::endl;
        for (auto& route : ipv4Routes) {
            str << route->ToString() << std::endl;
        }
        str << "IPv6 routes: " << std::endl;
        for (auto& route6 : ipv6Routes) {
            str << route6->ToString() << std::endl;
        }
        return str.str();
    }

    /****************************************** InterfaceInformation ******************************************/

}  // namespace net.
