// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#ifndef PROTOCOL_ANALYZER_API_HPP
#define PROTOCOL_ANALYZER_API_HPP

#define PROTOCOL_ANALYZER_FRAMEWORK_VERSION_MAJOR 0
#define PROTOCOL_ANALYZER_FRAMEWORK_VERSION_MINOR 0
#define PROTOCOL_ANALYZER_FRAMEWORK_VERSION_PATCH 11


#include "GlobalInfo.hpp"  // In this header file also defined "Log.hpp" and "Common.hpp".
#include "Mutex.hpp"
#include "System.hpp"
#include "LockedDeque.hpp"
#include "BinaryStructuredDataEngine.hpp"  // In this header file also defined "BinaryDataEngine.hpp".
#include "Parser.hpp"
#include "NetlinkInterface.hpp"  // In this header file also defined "Socket.hpp".
#include "Utilities.hpp"
#include "Notification.hpp"


///////////////////   STRUCTURE OF PROTOCOL ANALYZER FRAMEWORK   ///////////////////
/**
 * @namespace analyzer
 * @brief Main namespace that contains definition of analyzer.
 */
namespace analyzer
{
    /**
     * @namespace framework
     * @brief Namespace that contains definition of framework module of analyzer.
     */
    namespace framework
    {
        /**
         * @namespace system
         * @brief Namespace that contains definition of various core functions.
         */
        namespace system {
        }  // namespace system.
        /**
         * @namespace storage
         * @brief Namespace that contains definition of various framework global storages.
         */
        namespace storage {
        }  // namespace storage.
        /**
         * @namespace callbacks
         * @brief Namespace that contains interfaces of various framework callback types.
         */
        namespace callbacks {
        }  // namespace callbacks.
        /**
         * @namespace modules
         * @brief Namespace that contains identifiers of various framework modules.
         */
        namespace modules {
        }  // namespace modules.
        /**
         * @namespace net
         * @brief Namespace that contains definitions of network transports.
         */
        namespace net
        {
            /**
             * @namespace protocols
             * @brief Namespace that contains definitions of network transports and protocols.
             */
            namespace protocols
            {
                /**
                 * @namespace http
                 * @brief Namespace that contains definitions of HTTP protocols.
                 */
                namespace http {
                }  // namespace http.
            }  // namespace protocols.
        }  // namespace net.
        /**
         * @namespace utility
         * @brief Namespace that contains definition of various utilities.
         */
        namespace utility {
        }  // namespace utility.
        /**
         * @namespace common
         * @brief Namespace that contains definition of common functions.
         */
        namespace common
        {
            /**
             * @namespace types
             * @brief Namespace that contains definition of various framework types.
             */
            namespace types {
            }  // namespace types.
            /**
             * @namespace text
             * @brief Namespace that contains definition of various functions which works with text.
             */
            namespace text {
            }  // namespace text.
            /**
             * @namespace file
             * @brief Namespace that contains definition of various functions which works with files.
             */
            namespace file {
            }  // namespace file.
            /**
             * @namespace convert
             * @brief Namespace that contains definition of various functions which converts various types of data to the desired representation.
             */
            namespace convert {
            }  // namespace convert.
        }  // namespace common.
        /**
         * @namespace log
         * @brief Namespace that contains definition of logging and error classes.
         */
        namespace log {
        }  // namespace log.
        /**
         * @namespace task
         * @brief Namespace that contains definitions of task class.
         */
        namespace task {
        }  // namespace task.
        /**
         * @namespace parser
         * @brief Namespace that contains definitions of various parsers.
         */
        namespace parser{
        }  // namespace parser.
        /**
         * @namespace diagnostic
         * @brief Namespace that contains definitions of diagnostic tools.
         */
        namespace diagnostic {
        }  // namespace diagnostic.
    }  // namespace framework.
}  // namespace analyzer.
///////////////////   STRUCTURE OF PROTOCOL ANALYZER FRAMEWORK   ///////////////////

#endif  // PROTOCOL_ANALYZER_API_HPP
