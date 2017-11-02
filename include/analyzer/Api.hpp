#pragma once
#ifndef PROTOCOL_ANALYZER_API_HPP
#define PROTOCOL_ANALYZER_API_HPP

#define PROTOCOL_ANALYZER_VERSION_MAJOR 0
#define PROTOCOL_ANALYZER_VERSION_MINOR 0
#define PROTOCOL_ANALYZER_VERSION_PATCH 7

#include "Log.hpp"
#include "Socket.hpp"
//#include "Http.hpp"  // In this header file also defined "Protocol.hpp".
//#include "Task.hpp"
#include "Utilities.hpp"


///////////////////   STRUCTURE OF PROTOCOL ANALYZER   ///////////////////
/**
 * @namespace analyzer
 * @brief The main namespace that contains definition of analyzer framework.
 */
namespace analyzer
{
    /**
     * @namespace system
     * @brief The namespace that contains definition of various core functions.
     */
    namespace system {
    }  // namespace system.
    /**
     * @namespace net
     * @brief The namespace that contains definitions of network transports.
     */
    namespace net {
        /**
         * @namespace protocols
         * @brief The namespace that contains definitions of network transports and protocols.
         */
        namespace protocols {
            /**
             * @namespace http
             * @brief The namespace that contains definitions of HTTP protocols.
             */
            namespace http {
            }  // namespace http.
        }  // namespace protocols.
    }  // namespace net.
    /**
     * @namespace utility
     * @brief The namespace that contains definition of various utilities.
     */
    namespace utility {
    }  // namespace utility.
    /**
     * @namespace common
     * @brief The namespace that contains definition of common functions.
     */
    namespace common {
        /**
         * @namespace types
         * @brief The namespace that contains definition of various framework types.
         */
        namespace types {
        }  // namespace types.
        /**
         * @namespace text
         * @brief The namespace that contains definition of functions which works with text.
         */
        namespace text {
        }  // namespace text.
        /**
         * @namespace file
         * @brief The namespace that contains definition of functions which works with files.
         */
        namespace file {
        }  // namespace file.
    }  // namespace common.
    /**
     * @namespace log
     * @brief The namespace that contains definition of logging and error classes.
     */
    namespace log {
    }  // namespace log.
    /**
     * @namespace task
     * @brief The namespace that contains definitions of task class.
     */
    namespace task {
    }  // namespace task.
    /**
     * @namespace diagnostic
     * @brief The namespace that contains definitions of diagnostic tools.
     */
    namespace diagnostic {
    }  // namespace diagnostic.
    
}  // namespace analyzer.
///////////////////   STRUCTURE OF PROTOCOL ANALYZER   ///////////////////

#endif  // PROTOCOL_ANALYZER_API_HPP
