// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#include "../../include/framework/Log.hpp"  // In this header file also defined "Common.hpp".
#include "../../include/framework/Http.hpp"
#include "../../include/framework/Socket.hpp"

#define MAX_HTTP_HEADER_LENGTH 8192 // Maximum HTTP request header (Apache, Nginx, IIS)


namespace analyzer::framework::net::protocols::http
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
