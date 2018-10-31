// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#ifndef PROTOCOL_ANALYZER_HOST_ADDRESS_HPP
#define PROTOCOL_ANALYZER_HOST_ADDRESS_HPP

#include <ostream>  // std::ostream.
#include <cstring>  // memset, memcmp, memcpy.
#include <arpa/inet.h>  // inet_ntop, inet_pton.
#include <netinet/in.h>  // struct in_addr, struct in6_addr.


namespace analyzer::framework::net
{
    class IpAddress
    {
    public:
        in_addr ipv4 = { };
        in6_addr ipv6 = { };
        bool isIPv6 = false;
        bool exist = false;

    public:

        IpAddress(void) noexcept
            : isIPv6(false), exist(false)
        {
            ipv4.s_addr = 0;
            memset(&ipv6.s6_addr, 0, sizeof(in6_addr));
        }

        IpAddress (const uint32_t ip) noexcept
            : isIPv6(false), exist(true)
        {
            ipv4.s_addr = ip;
        }

        IpAddress (const sockaddr_in& ip) noexcept
            : ipv4(ip.sin_addr), isIPv6(false), exist(true)
        { }

        IpAddress (const sockaddr_in6& ip) noexcept
            : ipv6(ip.sin6_addr), isIPv6(true), exist(true)
        { }

        IpAddress (const in_addr& ip) noexcept
            : ipv4(ip), isIPv6(false), exist(true)
        { }

        IpAddress (const in6_addr& ip) noexcept
            : ipv6(ip), isIPv6(true), exist(true)
        { }


        IpAddress (IpAddress&& other) noexcept
        {
            if (other.exist == true)
            {
                exist = true;
                isIPv6 = other.isIPv6;
                if (isIPv6 == true)
                {
                    memcpy(ipv6.s6_addr, other.ipv6.s6_addr, sizeof(struct in6_addr));
                    memset(other.ipv6.s6_addr, 0, sizeof(struct in6_addr));
                    return;
                }
                memcpy(&ipv4.s_addr, &other.ipv4.s_addr, sizeof(struct in_addr));
                memset(&other.ipv4.s_addr, 0, sizeof(struct in_addr));
            }
        }

        IpAddress (const IpAddress& other)
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

        IpAddress& operator= (IpAddress&& other) noexcept
        {
            if (this != &other && other.exist == true)
            {
                exist = true;
                isIPv6 = other.isIPv6;
                if (isIPv6 == true)
                {
                    memcpy(ipv6.s6_addr, other.ipv6.s6_addr, sizeof(struct in6_addr));
                    memset(other.ipv6.s6_addr, 0, sizeof(struct in6_addr));
                    return *this;
                }
                memcpy(&ipv4.s_addr, &other.ipv4.s_addr, sizeof(struct in_addr));
                memset(&other.ipv4.s_addr, 0, sizeof(struct in_addr));
            }
            return *this;
        }

        IpAddress& operator= (const IpAddress& other)
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


        inline bool IsExist(void) const noexcept { return exist; }

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

        inline bool operator> (const IpAddress& other) const noexcept
        {
            return !(*this < other) && !(*this == other);
        }

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

        friend std::ostream& operator<< (std::ostream& out, const IpAddress& ip) noexcept
        {
            out << ip.ToString();
            return out;
        }

        inline uint16_t GetFamily(void) const noexcept
        {
            if (isIPv6 == true) {
                return AF_INET6;
            }
            return AF_INET;
        }

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

        bool AssignFromString (const uint16_t family, const std::string& ip) noexcept
        {
            if (family == AF_INET)
            {
                isIPv6 = false;
                const int32_t ret = inet_pton(AF_INET, ip.c_str(), &ipv4);
                if (ret == 1) { return true; }
            }
            else if (family == AF_INET6)
            {
                isIPv6 = true;
                const int32_t ret = inet_pton(AF_INET6, ip.c_str(), &ipv6);
                if (ret == 1) { return true; }
            }
            return false;
        }

        ~IpAddress(void) = default;
    };


    class MacAddress
    {
    private:
        uint8_t address[6];

    public:

        MacAddress(void) noexcept
        {
            memset(&address, 0, sizeof(address));
        }

        explicit MacAddress (const std::string& addr) noexcept
        {
            if (addr.size() != 6) { return; }
            memcpy(&address, addr.data(), sizeof(address));
        }

        explicit MacAddress (const char addr[6]) noexcept
        {
            memcpy(&address, addr, sizeof(address));
        }

        MacAddress (const MacAddress& other) noexcept
        {
            memcpy(address, other.address, sizeof(address));
        }

        MacAddress (MacAddress&& other) noexcept
        {
            memcpy(address, other.address, sizeof(address));
            memset(other.address, 0, sizeof(other.address));
        }

        MacAddress& operator= (const MacAddress& other) noexcept
        {
            if (this != &other) {
                memcpy(address, other.address, sizeof(address));
            }
            return *this;
        }

        MacAddress& operator= (MacAddress&& other) noexcept
        {
            if (this != &other) {
                memcpy(address, other.address, sizeof(address));
                memset(other.address, 0, sizeof(other.address));
            }
            return *this;
        }

        std::string ToString(void) const noexcept
        {
            char out[18] = { };
            snprintf(out, 18, "%02x:%02x:%02x:%02x:%02x:%02x", address[0], address[1], address[2], address[3], address[4], address[5]);
            return std::string(out);
        }
    };

}

#endif  // PROTOCOL_ANALYZER_HOST_ADDRESS_HPP
