#pragma once
#ifndef PROTOCOL_ANALYZER_PARSER_HPP
#define PROTOCOL_ANALYZER_PARSER_HPP

#include <vector>
#include <string_view>


namespace analyzer::parser
{
    /**
     * @class PortsParser Common.hpp "include/analyzer/Common.hpp"
     * @brief Class that parses the range of ports.
     */
    class PortsParser
    {
    private:
        /**
         * @var uint16_t rangeEnd;
         * @brief Variable that contains the last value of range.
         */
        uint16_t rangeEnd = 0;
        /**
         * @var uint16_t rangeStart;
         * @brief Variable that contains the first value of range.
         */
        uint16_t rangeState = 0;
        /**
         * @var std::vector<std::string_view> states;
         * @brief Vector of strings that contains the split values on input.
         */
        std::vector<std::string_view> states = { };

    public:
        /**
         * @fn PortsParser::PortsParser(void);
         * @brief Protection default constructor.
         */
        PortsParser(void) = default;
        /**
         * @fn PortsParser::~PortsParser(void);
         * @brief Protection default destructor.
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
         * @fn explicit PortsParser (std::string_view, char) noexcept;
         * @brief Constructor of PortParser class.
         * @param [in] ports - The sequence of ports listed through a separator.
         * @param [in] symbol - The separator. Default: ','.
         */
        explicit PortsParser (std::string_view /*ports*/, char /*symbol*/ = ',') noexcept;

        /**
         * @fn void SetPorts (std::string_view, char) noexcept;
         * @brief Method that resets internal state of port parser.
         * @param [in] ports - The sequence of ports listed through a separator.
         * @param [in] symbol - The separator. Default: ','.
         */
        void SetPorts (std::string_view /*ports*/, char /*symbol*/ = ',') noexcept;

        /**
         * @fn uint16_t GetNextPort(void) noexcept;
         * @brief Method that gets next parsing port.
         */
        uint16_t GetNextPort(void) noexcept;
    };

}  // namespace parser.


#endif  // PROTOCOL_ANALYZER_PARSER_HPP
