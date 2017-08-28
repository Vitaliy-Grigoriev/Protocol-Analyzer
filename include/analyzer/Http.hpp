#pragma once
#ifndef PROTOCOL_ANALYZER_HTTP_HPP
#define PROTOCOL_ANALYZER_HTTP_HPP

#include "Protocol.hpp"


namespace analyzer::net::protocols::http
{
    /**
     * @enum HTTP_VERSION
     * @brief The versions of the HTTP protocols.
     */
    enum HTTP_VERSION : uint16_t
    {
        UNKNOWN = 0,
        HTTP1_1 = 1,
        HTTP2_0 = 2
    };


    /**
     * @class HttpHeader Http.hpp "include/analyzer/Http.hpp"
     * @brief HTTP/1.1 and HTTP/2.0 headers class.
     */
    class HttpHeader {


    };


    /**
     * @class http Http.hpp "include/analyzer/Http.hpp"
     * @brief HTTP/1.1 and HTTP/2.0 implementations.
     */
    class http : virtual public Protocol {
    private:


    public:
        /**
         * @fn http (void *, const std::size_t, HTTP_VERSION = HTTP1_1);
         * @brief Constructor of HTTP protocol class.
         * @param [in] data - The raw data from the socket.
         * @param [in] length - The size of the raw data in bytes.
         * @param [in] HTTP_VERSION - The version of the considered protocol.
         */
        http (const void * /*data*/, const std::size_t /*length*/, HTTP_VERSION /*version*/ = HTTP1_1);

        /**
         * @fn http (const std::string &, HTTP_VERSION);
         * @brief Constructor of HTTP protocol class.
         * @param [in] data - The raw data from the socket.
         * @param [in] HTTP_VERSION - The version of the considered protocol.
         */
        http (const std::string & /*data*/, HTTP_VERSION /*version*/ = HTTP1_1);

        /**
         * @fn void ParseHeader(void) override final;
         * @brief Parsing the HTTP/1.1 and HTTP/2.0 header.
         */
        void ParseHeader(void) override final;

        /**
         * @fn ~http(void);
         * @brief Destructor of http class.
         */
        ~http(void);
    };

}  // namespace http.


#endif  // PROTOCOL_ANALYZER_HTTP_HPP
