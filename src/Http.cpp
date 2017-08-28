// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "../include/analyzer/Log.hpp"  // In this header file also defined "Common.hpp".
#include "../include/analyzer/Http.hpp"
#include "../include/analyzer/Socket.hpp"

#define MAX_HTTP_HEADER_LENGTH 8192 // Maximum HTTP request header (Apache, Nginx, IIS)


namespace analyzer::net::protocols::http
{
    http::http (const void* data, const std::size_t length, HTTP_VERSION version)
    {
        auto iterator = static_cast<const char*>(data);
    }

    http::http (const std::string& data, HTTP_VERSION version)
            : http(static_cast<const void*>(data.data()), data.size(), version)
    { }

    bool http::ParseHeader(void)
    {
        return true;
    }

    http::~http(void) = default;

}  // namespace http.
