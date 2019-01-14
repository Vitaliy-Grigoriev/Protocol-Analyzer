// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#ifndef PROTOCOL_ANALYZER_SCANNER_API_HPP
#define PROTOCOL_ANALYZER_SCANNER_API_HPP

#define PROTOCOL_ANALYZER_SCANNER_VERSION_MAJOR 0
#define PROTOCOL_ANALYZER_SCANNER_VERSION_MINOR 0
#define PROTOCOL_ANALYZER_SCANNER_VERSION_PATCH 1


#include "ArgumentsParser.hpp"  // settings::ArgumentsParser.


///////////////////   STRUCTURE OF PROTOCOL ANALYZER SCANNER   ///////////////////
/**
 * @namespace analyzer
 * @brief Main namespace that contains definition of analyzer.
 */
namespace analyzer
{
    /**
     * @namespace scanner
     * @brief Namespace that contains definition of scanner module of analyzer.
     */
    namespace scanner
    {
        /**
         * @namespace system
         * @brief Namespace that contains definition of several classes which works with program and protocol settings.
         */
        namespace settings {
        }  // namespace settings.
    }  // namespace scanner.
}  // namespace analyzer.
///////////////////   STRUCTURE OF PROTOCOL ANALYZER SCANNER   ///////////////////


#endif  // PROTOCOL_ANALYZER_SCANNER_API_HPP
