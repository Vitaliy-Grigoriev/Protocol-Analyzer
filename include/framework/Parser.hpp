// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#ifndef PROTOCOL_ANALYZER_PARSER_HPP
#define PROTOCOL_ANALYZER_PARSER_HPP

#include <vector>
#include <string_view>


namespace analyzer::parser
{
    /**
     * @class PortsParser Parser.hpp "include/framework/Parser.hpp"
     * @brief Class that parses the range of ports.
     */
    class PortsParser
    {
    private:
        /**
         * @var uint16_t rangeState;
         * @brief Variable that contains the current value of range.
         */
        uint16_t rangeState = 0;
        /**
         * @var uint16_t rangeEnd;
         * @brief Variable that contains the last value of range.
         */
        uint16_t rangeEnd = 0;
        /**
         * @var std::vector<std::string_view> inputStates;
         * @brief Vector of strings that contains the split values on input.
         */
        std::vector<std::string_view> inputStates = { };

    public:
        /**
         * @fn PortsParser::PortsParser(void);
         * @brief Default constructor.
         */
        PortsParser(void) = default;
        /**
         * @fn PortsParser::~PortsParser(void);
         * @brief Default destructor.
         */
        ~PortsParser(void) = default;

        PortsParser (PortsParser &&) = delete;
        PortsParser (const PortsParser &) = delete;
        PortsParser & operator= (PortsParser &&) = delete;
        PortsParser & operator= (const PortsParser &) = delete;

        /**
         * @var static const uint16_t end;
         * @brief Static variable that indicates the end of parsing or error.
         */
        static const uint16_t end = 0;

        /**
         * @fn explicit PortsParser::PortsParser (std::string_view, char) noexcept;
         * @brief Constructor of PortParser class.
         * @param [in] ports - The sequence of ports listed through a separator.
         * @param [in] delimiter - The separator. Default: ','.
         *
         * @note For example: 80,1-5,433,25-36,90.
         */
        explicit PortsParser (std::string_view /*ports*/, char /*delimiter*/ = ',') noexcept;

        /**
         * @fn void PortsParser::SetPorts (std::string_view, char) noexcept;
         * @brief Method that sets internal state of port parser.
         * @param [in] ports - The sequence of ports listed through a separator.
         * @param [in] delimiter - The separator. Default: ','.
         */
        void SetPorts (std::string_view /*ports*/, char /*delimiter*/ = ',') noexcept;

        /**
         * @fn uint16_t PortsParser::GetNextPort(void) noexcept;
         * @brief Method that gets next port value in input range.
         * @return Port number or PortsParser::end value if port enumeration is complete or an error occurred.
         *
         * @note The program MUST check the return value for PortsParser::end value.
         */
        uint16_t GetNextPort(void) noexcept;
    };

}  // namespace parser.


#endif  // PROTOCOL_ANALYZER_PARSER_HPP
