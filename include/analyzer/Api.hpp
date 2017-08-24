#pragma once
#ifndef HTTP2_ANALYZER_API_HPP
#define HTTP2_ANALYZER_API_HPP

#define ANALYZER_VERSION_MAJOR 0
#define ANALYZER_VERSION_MINOR 0
#define ANALYZER_VERSION_PATCH 5

#include "Socket.hpp" // In this header file also defined "Common.hpp".
#include "Http.hpp"   // In this header file also defined "Protocol.hpp".
#include "Task.hpp"
#include "Utilities.hpp"


///////////////////   STRUCTURE OF ANALYZER   ///////////////////
/**
 * @namespace analyzer
 * @brief The main namespace that contain definition of analyzer framework.
 */
namespace analyzer {
    /**
     * @namespace net
     * @brief The namespace that contain definitions of network transports.
     */
    namespace net {
        /**
         * @namespace protocols
         * @brief The namespace that contain definitions of network protocols.
         */
        namespace protocols {
            /**
             * @namespace http
             * @brief The namespace that contain definitions of HTTP protocols.
             */
            namespace http {
            }  // namespace http.
        }  // namespace protocols.
    }  // namespace net.
    /**
     * @namespace utility
     * @brief The namespace that contain definition of various utilities.
     */
    namespace utility {
    }  // namespace utility.
    /**
     * @namespace common
     * @brief The namespace that contain definition of common functions.
     */
    namespace common {
    }  // namespace common.
    /**
     * @namespace log
     * @brief The namespace that contain definition of logging and error classes.
     */
    namespace log {
    }  // namespace log.
    /**
     * @namespace task
     * @brief The namespace that contain definitions of task class.
     */
    namespace task {
    }  // namespace task.
    /**
     * @namespace diagnostic
     * @brief The namespace that contain definitions of diagnostic tools.
     */
    namespace diagnostic {
    }  // namespace diagnostic.
}  // namespace analyzer.
///////////////////   STRUCTURE OF ANALYZER   ///////////////////

#endif  // HTTP2_ANALYZER_API_HPP
