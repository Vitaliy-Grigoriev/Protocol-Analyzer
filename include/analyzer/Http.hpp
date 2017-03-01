#pragma once
#ifndef HTTP2_ANALYZER_HTTP_HPP
#define HTTP2_ANALYZER_HTTP_HPP


#include "Log.hpp"
#include "ProtocolLayer.hpp"


namespace analyzer {
    namespace protocol {
        /**
         * @namespace http
         * @brief The namespace that contain definitions of http protocols.
         */
        namespace http {
            /**
             * @enum HTTP_VERSION
             * @brief The versions of the HTTP protocols.
             */
            enum HTTP_VERSION : uint16_t {
                UNKNOWN = 0, HTTP1_1 = 1, HTTP2_0 = 2
            };


            /**
             * @class HttpHeader Http.hpp "include/analyzer/Http.hpp"
             * @brief HTTP/1.1 and HTTP/2.0 headers.
             */
            class HttpHeader {



            };


            /**
             * @class http Http.hpp "include/analyzer/Http.hpp"
             * @brief HTTP/1.1 and HTTP/2.0 implementations.
             */
            class http : virtual public ProtocolLayer {
            private:


            public:
                /**
                 * @fn explicit http (void *, const std::size_t, HTTP_VERSION);
                 * @brief Constructor of HTTP protocol class.
                 * @param [in] data - The raw data from the socket.
                 * @param [in] length - The size of the raw data in bytes.
                 * @param [in] HTTP_VERSION - The version of the considered protocol.
                 */
                explicit http (void * /*data*/, const std::size_t /*length*/, HTTP_VERSION /*version*/);
                /**
                 * @fn explicit http (const std::string &, HTTP_VERSION);
                 * @brief Constructor of HTTP protocol class.
                 * @param [in] data - The raw data from the socket.
                 * @param [in] HTTP_VERSION - The version of the considered protocol.
                 */
                explicit http (const std::string & /*data*/, HTTP_VERSION /*version*/);

                // Parsing the HTTP/1.1 and HTTP/2.0 header.
                void ParseHeader(void) override final;

                // Destructor.
                ~http(void);
            };

        }  // namespace http.
    }  // namespace protocol.
}  // namespace analyzer.


#endif  // HTTP2_ANALYZER_HTTP_HPP
