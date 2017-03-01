// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "../include/analyzer/Http.hpp"

namespace analyzer {
    namespace protocol {
        namespace http {

            http::http (void* data, const std::size_t length, HTTP_VERSION version)
            {
                const char* iterator = static_cast<const char*>(data);
            }

            http::http (const std::string& data, HTTP_VERSION version)
            { }

            void http::ParseHeader(void)
            { }

            http::~http(void)
            { }


        }  // namespace http.

        ProtocolLayer::~ProtocolLayer(void) { }

    }  // namespace protocol.
}  // namespace analyzer.
