// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#ifndef PROTOCOL_ANALYZER_NETWORK_TYPES_HPP
#define PROTOCOL_ANALYZER_NETWORK_TYPES_HPP

#include <list>  // std::list.
#include <string>  // std::string.
#include <cstdlib>  // std::strtoul.
#include <ostream>  // std::ostream.
#include <cstring>  // memset, memcmp, memcpy, strtok_r.
#include <functional>  // std::hash.
#include <arpa/inet.h>  // inet_ntop, inet_pton.
#include <netinet/in.h>  // struct in_addr, struct in6_addr, struct sockaddr_storage.
#include <linux/rtnetlink.h>  // struct ifinfomsg, struct ifaddrmsg, struct rtmsg.


namespace analyzer::framework::net
{
    /**
     * @struct IpAddress   NetworkTypes.hpp   "include/framework/NetworkTypes.hpp"
     * @brief This structure defined the interface for work with network IPv4 and IPv6 addresses.
     */
    struct IpAddress
    {
    public:
        /**
         * @brief Variable which contains IPv4 address.
         */
        in_addr ipv4 = { };
        /**
         * @brief Variable which contains IPv6 address.
         */
        in6_addr ipv6 = { };
        /**
         * @brief Boolean value that indicates about the type of stored IP address.
         */
        bool isIPv6 = false;
        /**
         * @brief Boolean value that indicates about the existence of IP address.
         */
        bool exist = false;

    public:
        IpAddress(void) = default;
        ~IpAddress(void) = default;

        /**
         * @brief Constructor that initializes IPv4 address by 32-bit value.
         *
         * @param [in] ip - IPv4 address.
         */
        explicit IpAddress (const uint32_t ip) noexcept
            : isIPv6(false), exist(true)
        {
            ipv4.s_addr = ip;
        }

        /**
         * @brief Constructor that initializes IPv4 address by 'sockaddr_in' structure.
         *
         * @param [in] ip - IPv4 address in 'sockaddr_in' representation.
         */
        explicit IpAddress (const sockaddr_in& ip) noexcept
            : ipv4(ip.sin_addr), isIPv6(false), exist(true)
        { }

        /**
         * @brief Constructor that initializes IPv6 address by 'sockaddr_in6' structure.
         *
         * @param [in] ip - IPv6 address in 'sockaddr_in6' representation.
         */
        explicit IpAddress (const sockaddr_in6& ip) noexcept
            : ipv6(ip.sin6_addr), isIPv6(true), exist(true)
        { }

        /**
         * @brief Constructor that initializes IPv4 address by 'in_addr' structure.
         *
         * @param [in] ip - IPv4 address in 'in_addr' representation.
         */
        explicit IpAddress (const in_addr& ip) noexcept
            : ipv4(ip), isIPv6(false), exist(true)
        { }

        /**
         * @brief Constructor that initializes IPv6 address by 'in_addr6' structure.
         *
         * @param [in] ip - IPv6 address in 'in_addr6' representation.
         */
        explicit IpAddress (const in6_addr& ip) noexcept
            : ipv6(ip), isIPv6(true), exist(true)
        { }

        /**
         * @brief Copy assignment constructor of IpAddress class.
         *
         * @param [in] other - Constant lvalue reference of copied IpAddress class.
         */
        IpAddress (const IpAddress & /*other*/) noexcept;

        /**
         * @brief Move assignment constructor of IpAddress class.
         *
         * @param [in] other - Rvalue reference of moved IpAddress class.
         */
        IpAddress (IpAddress && /*other*/) noexcept;

        /**
         * @brief Copy assignment operator of IpAddress class.
         *
         * @param [in] other - Constant lvalue reference of copied IpAddress class.
         */
        IpAddress & operator= (const IpAddress & /*other*/) noexcept;

        /**
         * @brief Move assignment operator of IpAddress class.
         *
         * @param [in] other - Rvalue reference of moved IpAddress class.
         */
        IpAddress & operator= (IpAddress && /*other*/) noexcept;

        /**
         * @brief Method that returns the indicator of existence of IP address.
         *
         * @return TRUE - if IP address is exist, oterwise - FALSE.
         */
        inline bool IsExist(void) const noexcept { return exist; }

        /**
         * @brief Method that return the result of comparison of the two IP addresses.
         *
         * @param [in] other - Constant lvalue reference of other IpAddress class.
         * @return TRUE - if stored IP address less then other, otherwise - FALSE.
         */
        bool operator< (const IpAddress & /*other*/) const noexcept;

        /**
         * @brief Method that return the result of comparison of the two IP addresses.
         *
         * @param [in] other - Constant lvalue reference of other IpAddress class.
         * @return TRUE - if stored IP address more then other, otherwise - FALSE.
         */
        bool operator> (const IpAddress & /*other*/) const noexcept;

        /**
         * @brief Method that return the result of comparison of the two IP addresses.
         *
         * @param [in] other - Constant lvalue reference of other IpAddress class.
         * @return TRUE - if stored IP address is equal with other, otherwise - FALSE.
         */
        bool operator== (const IpAddress & /*other*/) const noexcept;

        /**
         * @brief Operator that outputs IP address in string format.
         *
         * @param [in,out] stream - Reference of the output stream engine.
         * @param [in] ip - Constant lvalue reference of IpAddress class.
         * @return Lvalue reference of the inputted STL std::ostream class.
         */
        friend std::ostream& operator<< (std::ostream& stream, const IpAddress& ip) noexcept
        {
            stream << ip.ToString();
            return stream;
        }

        /**
         * @brief Method that returns the network family of stored IP address.
         *
         * @return Network family of stored IP address.
         */
        inline uint16_t GetFamily(void) const noexcept
        {
            return static_cast<uint16_t>((isIPv6 == false) ? AF_INET : AF_INET6);
        }

        /**
         * @brief Method that returns stored IP address in 'sockaddr_storage' representation.
         *
         * @return IP address in 'sockaddr_storage' representation.
         */
        struct sockaddr_storage GetSockaddr(void) const noexcept;

        /**
         * @brief Method that returns IP address in string format.
         *
         * @return IP address in string format.
         */
        std::string ToString(void) const noexcept;

        /**
         * @brief Method that assigns IP address from string format.
         *
         * @param [in] family - Network family of inputted IP address.
         * @param [in] ip - Value of the IP address in string representation.
         * @return Boolean value that indicates about the status of the assignment.
         */
        bool FromString (uint16_t /*family*/, const std::string & /*ip*/) noexcept;

    };



    /**
     * @struct MacAddress   NetworkTypes.hpp   "include/framework/NetworkTypes.hpp"
     * @brief This structure defined the interface for work with network MAC address.
     */
    struct MacAddress
    {
    public:
        /**
         * @brief Array that stores the bytes of MAC address.
         */
        uint8_t address[6] = { };

    public:
        MacAddress(void) = default;
        ~MacAddress(void) = default;

        /**
         * @brief Constructor that initializes MAC address.
         *
         * @param [in] addr - MAC address in byte representation.
         */
        explicit MacAddress (const char[6] /*addr*/) noexcept;

        /**
         * @brief Copy assignment constructor of MacAddress class.
         *
         * @param [in] other - Constant lvalue reference of copied MacAddress class.
         */
        MacAddress (const MacAddress & /*other*/) noexcept;

        /**
         * @brief Move assignment constructor of MacAddress class.
         *
         * @param [in] other - Rvalue reference of moved MacAddress class.
         */
        MacAddress (MacAddress && /*other*/) noexcept;

        /**
         * @brief Copy assignment operator of MacAddress class.
         *
         * @param [in] other - Constant lvalue reference of copied MacAddress class.
         */
        MacAddress & operator= (const MacAddress & /*other*/) noexcept;

        /**
         * @brief Move assignment operator of MacAddress class.
         *
         * @param [in] other - Rvalue reference of moved MacAddress class.
         */
        MacAddress& operator= (MacAddress && /*other*/) noexcept;

        /**
         * @brief Method that returns MAC address in string format.
         *
         * @return MAC address in string format.
         */
        std::string ToString(void) const noexcept;

        /**
         * @brief Method that assigns MAC address from string format.
         *
         * @param [in] mac - Value of the MAC address in string representation.
         * @return Boolean value that indicates about the status of the assignment.
         */
        bool FromString (const std::string & /*mac*/) noexcept;
    };


    /**
     * @struct InterfaceAddresses   NetworkTypes.hpp   "include/framework/NetworkTypes.hpp"
     * @brief This structure defined an interface IP addresses.
     */
    struct InterfaceAddresses
    {
        IpAddress unicastIpAddress;
        IpAddress localIpAddress;
        IpAddress anycastIpAddress;
        IpAddress broadcastIpAddress;
        IpAddress multicastIpAddress;

        /**
         * @brief Method that outputs the data of InterfaceAddresses structure in string format.
         *
         * @return InterfaceAddresses data in string representation.
         */
        std::string ToString(void) const noexcept;
    };

    /**
     * @struct InterfaceInformation   NetworkTypes.hpp   "include/framework/NetworkTypes.hpp"
     * @brief Forward declaration of InterfaceInformation structure.
     */
    struct InterfaceInformation;

    /**
     * @struct RouteInformation   NetworkTypes.hpp   "include/framework/NetworkTypes.hpp"
     * @brief This structure defined network route.
     */
    struct RouteInformation
    {
        IpAddress sourceAddress;
        IpAddress destinationAddress;
        IpAddress destinationMask;
        IpAddress gatewayAddress;

        bool isDefault;
        uint8_t routeScope;
        uint8_t routeFamily;
        uint8_t routeType;
        int32_t routePriority;
        uint32_t interfaceIndex;
        struct InterfaceInformation * ownerInterface;

        /**
         * @brief Method that outputs the data of RouteInformation structure in string format.
         *
         * @return RouteInformation data in string representation.
         */
        std::string ToString(void) const noexcept;
    };

    /**
     * @struct InterfaceInformation   NetworkTypes.hpp   "include/framework/NetworkTypes.hpp"
     * @brief This structure defined network interface.
     */
    struct InterfaceInformation
    {
        std::string interfaceName;
        uint32_t interfaceType;
        uint32_t interfaceIndex;
        uint32_t mtuSize;
        uint8_t interfaceFamily;
        MacAddress macAddress;
        MacAddress broadcastMacAddress;

        std::list<InterfaceAddresses> ipv4Addresses;
        std::list<InterfaceAddresses> ipv6Addresses;

        RouteInformation * defaultIpv4Route;
        std::list<RouteInformation*> ipv4Routes;
        std::list<RouteInformation*> ipv6Routes;

        /**
         * @brief Method that outputs the data of InterfaceInformation structure in string format.
         *
         * @return InterfaceInformation data in string representation.
         */
        std::string ToString(void) const noexcept;
    };
}


namespace std
{
    /**
     * @struct hash<net::IpAddress>   NetworkTypes.hpp   "analyzer/framework/NetworkTypes.hpp"
     * @brief Specialize the std::hash template for a IpAddress class for using in hash tables.
     */
    template <>
    struct hash <analyzer::framework::net::IpAddress>
    {
        std::size_t operator() (const analyzer::framework::net::IpAddress& value) const noexcept
        {
            if (value.isIPv6 == false) {
                return std::hash<uint32_t>()(value.ipv4.s_addr);
            }
            const uint64_t* part1 = reinterpret_cast<const uint64_t*>(&value.ipv6.s6_addr[0]);
            const uint64_t* part2 = reinterpret_cast<const uint64_t*>(&value.ipv6.s6_addr[8]);
            return std::hash<uint64_t>()(*part1) ^ std::hash<uint64_t>()(*part2);
        }
    };

    /**
     * @struct hash<net::MacAddress>   NetworkTypes.hpp   "analyzer/framework/NetworkTypes.hpp"
     * @brief Specialize the std::hash template for a MacAddress class for using in hash tables.
     */
    template <>
    struct hash <analyzer::framework::net::MacAddress>
    {
        std::size_t operator() (const analyzer::framework::net::MacAddress& value) const noexcept
        {
            // First three bytes determine the manufacturer, so for hash is used last four bytes.
            return std::hash<uint32_t>()(*reinterpret_cast<const uint32_t*>(value.address[2]));
        }
    };
}

#endif  // PROTOCOL_ANALYZER_NETWORK_TYPES_HPP
