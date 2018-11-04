// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#ifndef PROTOCOL_ANALYZER_NETWORK_ADDRESSES_HPP
#define PROTOCOL_ANALYZER_NETWORK_ADDRESSES_HPP

#include <string>  // std::string.
#include <cstdlib>  // std::strtoul.
#include <ostream>  // std::ostream.
#include <cstring>  // memset, memcmp, memcpy, strtok_r.
#include <limits.h>  // ULONG_MAX.
#include <arpa/inet.h>  // inet_ntop, inet_pton.
#include <netinet/in.h>  // struct in_addr, struct in6_addr, struct sockaddr_storage.


namespace analyzer::framework::net
{
    /**
     * @struct IpAddress   NetworkAddresses.hpp   "include/framework/NetworkAddresses.hpp"
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
         * @param [in] other - Const lvalue reference of copied IpAddress class.
         */
        IpAddress (const IpAddress& other) noexcept
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

        /**
         * @brief Move assignment constructor of IpAddress class.
         *
         * @param [in] other - Rvalue reference of moved IpAddress class.
         */
        IpAddress (IpAddress&& other) noexcept
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

        /**
         * @brief Copy assignment operator of IpAddress class.
         *
         * @param [in] other - Const lvalue reference of copied IpAddress class.
         */
        IpAddress& operator= (const IpAddress& other) noexcept
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

        /**
         * @brief Move assignment operator of IpAddress class.
         *
         * @param [in] other - Rvalue reference of moved IpAddress class.
         */
        IpAddress& operator= (IpAddress&& other) noexcept
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

        /**
         * @brief Method that returns the indicator of existence of IP address.
         *
         * @return TRUE - if IP address is exist, oterwise - FALSE.
         */
        inline bool IsExist(void) const noexcept { return exist; }

        /**
         * @brief Method that return the result of comparison of the two IP addresses.
         *
         * @param [in] other - Const lvalue reference of other IpAddress class.
         * @return TRUE - if stored IP address less then other, otherwise - FALSE.
         */
        inline bool operator< (const IpAddress& other) const noexcept
        {
            if (isIPv6 != other.isIPv6) {
                return !isIPv6;
            }

            if (isIPv6 == true) {
                return (memcmp(ipv6.s6_addr, other.ipv6.s6_addr, sizeof(ipv6)) < 0);
            }
            return (ipv4.s_addr < other.ipv4.s_addr);
        }

        /**
         * @brief Method that return the result of comparison of the two IP addresses.
         *
         * @param [in] other - Const lvalue reference of other IpAddress class.
         * @return TRUE - if stored IP address more then other, otherwise - FALSE.
         */
        inline bool operator> (const IpAddress& other) const noexcept
        {
            return !(*this < other) && !(*this == other);
        }

        /**
         * @brief Method that return the result of comparison of the two IP addresses.
         *
         * @param [in] other - Const lvalue reference of other IpAddress class.
         * @return TRUE - if stored IP address is equal with other, otherwise - FALSE.
         */
        inline bool operator== (const IpAddress & other) const noexcept
        {
            if (isIPv6 != other.isIPv6) {
                return false;
            }

            if (isIPv6 == true) {
                return (memcmp(ipv6.s6_addr, other.ipv6.s6_addr, sizeof(ipv6)) == 0);
            }
            return (ipv4.s_addr == other.ipv4.s_addr);
        }

        /**
         * @brief Operator that outputs IP address in string format.
         *
         * @param [in,out] stream - Reference of the output stream engine.
         * @param [in] ip - Const lvalue reference of IpAddress class.
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
        struct sockaddr_storage GetSockaddr(void) const noexcept
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

        /**
         * @brief Method that returns IP address in string format.
         *
         * @return IP address in string format.
         */
        std::string ToString(void) const noexcept
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

        /**
         * @brief Method that assigns IP address from string format.
         *
         * @param [in] family - Network family of inputted IP address.
         * @param [in] ip - Value of the IP address in string representation.
         * @return Boolean value that indicates about the status of the assignment.
         */
        bool FromString (const uint16_t family, const std::string& ip) noexcept
        {
            exist = true;
            if (family == AF_INET) {
                isIPv6 = false;
                return (inet_pton(AF_INET, ip.c_str(), &ipv4) == 1);
            }

            isIPv6 = true;
            return (inet_pton(AF_INET6, ip.c_str(), &ipv6) == 1);
        }
    };



    /**
     * @struct MacAddress   NetworkAddresses.hpp   "include/framework/NetworkAddresses.hpp"
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
        explicit MacAddress (const char addr[6]) noexcept
        {
            memcpy(&address, addr, sizeof(address));
        }

        /**
         * @brief Copy assignment constructor of MacAddress class.
         *
         * @param [in] other - Const lvalue reference of copied MacAddress class.
         */
        MacAddress (const MacAddress& other) noexcept
        {
            memcpy(address, other.address, sizeof(address));
        }

        /**
         * @brief Move assignment constructor of MacAddress class.
         *
         * @param [in] other - Rvalue reference of moved MacAddress class.
         */
        MacAddress (MacAddress&& other) noexcept
        {
            memcpy(address, other.address, sizeof(address));
            memset(other.address, 0x00, sizeof(other.address));
        }

        /**
         * @brief Copy assignment operator of MacAddress class.
         *
         * @param [in] other - Const lvalue reference of copied MacAddress class.
         */
        MacAddress& operator= (const MacAddress& other) noexcept
        {
            if (this != &other) {
                memcpy(address, other.address, sizeof(address));
            }
            return *this;
        }

        /**
         * @brief Move assignment operator of MacAddress class.
         *
         * @param [in] other - Rvalue reference of moved MacAddress class.
         */
        MacAddress& operator= (MacAddress&& other) noexcept
        {
            if (this != &other) {
                memcpy(address, other.address, sizeof(address));
                memset(other.address, 0x00, sizeof(other.address));
            }
            return *this;
        }

        /**
         * @brief Method that returns MAC address in string format.
         *
         * @return MAC address in string format.
         */
        std::string ToString(void) const noexcept
        {
            char out[18] = { };
            snprintf(out, 18, "%02x:%02x:%02x:%02x:%02x:%02x", address[0], address[1], address[2], address[3], address[4], address[5]);
            return std::string(out);
        }

        /**
         * @brief Method that assigns MAC address from string format.
         *
         * @param [in] mac - Value of the MAC address in string representation.
         * @return Boolean value that indicates about the status of the assignment.
         */
        bool FromString (const std::string& mac) noexcept
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
    };

}

#endif  // PROTOCOL_ANALYZER_NETWORK_ADDRESSES_HPP
