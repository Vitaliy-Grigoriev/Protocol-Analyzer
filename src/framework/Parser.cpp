// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "../../include/framework/Log.hpp"  // In this header file also defined "Common.hpp".
#include "../../include/framework/Parser.hpp"


namespace analyzer::framework::parser
{
    PortsParser::PortsParser (std::string_view ports, const char delimiter) noexcept
    {
        SetPorts(ports, delimiter);
    }

    void PortsParser::SetPorts (std::string_view ports, const char delimiter) noexcept
    {
        inputStates = common::text::splitInPlace(ports, delimiter);
        rangeState = rangeEnd = end;
    }

    uint16_t PortsParser::GetNextPort(void) noexcept
    {
        if (inputStates.empty() == true) {
            return end;
        }

        // If current observed state is not a range.
        if (rangeEnd == end)
        {
            const std::string_view nextPortValue = inputStates.at(0);
            const std::size_t position = nextPortValue.find('-');

            try
            {
                // If a range delimiter '-' not found then a single port value is parsed.
                if (position == std::string_view::npos)
                {
                    const uint64_t port = std::stoull(&nextPortValue[0], nullptr, 10);
                    if (port < 65536) {
                        inputStates.erase(inputStates.cbegin());
                        return static_cast<uint16_t>(port);
                    }
                    inputStates.clear();
                    return end;
                }

                // In this case the port range is parsed and the first port number is returned.
                const uint64_t portStart = std::stoull(&nextPortValue[0], nullptr, 10);
                const uint64_t portEnd = std::stoull(&nextPortValue[position + 1], nullptr, 10);
                if (portStart < portEnd && portEnd < 65536) {
                    rangeState = static_cast<uint16_t>(portStart);
                    rangeEnd = static_cast<uint16_t>(portEnd);
                    return rangeState;
                }
            }
            catch (const std::invalid_argument& err) {
                LOG_ERROR("PortsParser.GetNextPort: Invalid argument - '", nextPortValue, "' (", err.what(), ").");
            }
            catch (const std::out_of_range& err) {
                LOG_ERROR("PortsParser.GetNextPort: Out of range - '", nextPortValue, "' (", err.what(), ").");
            }
            inputStates.clear();
            return end;
        }

        // In this case the next value in the range is returned.
        if (++rangeState == rangeEnd) {
            inputStates.erase(inputStates.cbegin());
            rangeEnd = end;
        }
        return rangeState;
    }

}  // namespace parser.
