// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "../include/analyzer/Log.hpp"
#include "../include/analyzer/Parser.hpp"


namespace analyzer::parser
{
    PortsParser::PortsParser (std::string_view ports, const char symbol) noexcept
    {
        SetPorts(ports, symbol);
    }

    void PortsParser::SetPorts (std::string_view ports, const char symbol) noexcept
    {
        states = common::text::splitInPlace(ports, symbol);
        rangeState = rangeEnd = 0;
    }

    uint16_t PortsParser::GetNextPort(void) noexcept
    {
        if (states.empty() == true) {
            return end;
        }

        if (rangeEnd == end)
        {
            std::string_view value = states.at(0);
            const std::size_t position = value.find('-');

            try
            {
                if (position == std::string_view::npos)
                {
                    const uint64_t port = std::stoul(&value[0], nullptr, 10);
                    if (port < 65536) {
                        states.erase(states.begin());
                        return static_cast<uint16_t>(port);
                    }
                    states.clear();
                    return end;
                }

                const uint64_t portStart = std::stoul(&value[0], nullptr, 10);
                const uint64_t portEnd = std::stoul(&value[position + 1], nullptr, 10);
                if (portStart < portEnd && portEnd < 65536) {
                    rangeState = static_cast<uint16_t>(portStart);
                    rangeEnd = static_cast<uint16_t>(portEnd);
                    return rangeState;
                }
            }
            catch (const std::exception& err)
            {
                LOG_ERROR("PortsParser.GetNextPort: Exception occurred when parse ports (", value, "): ", err.what(), '.');
                states.clear();
                return end;
            }
        }

        if (++rangeState == rangeEnd) {
            states.erase(states.begin());
            rangeEnd = end;
        }
        return rangeState;
    }

}  // namespace parser.
