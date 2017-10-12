// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "../include/analyzer/Common.hpp"


namespace analyzer::common
{
    void trim_left (std::string& str) noexcept
    {
        str.erase(str.begin(), std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun<int32_t, int32_t>(std::isspace))));
    }

    void trim_right (std::string& str) noexcept
    {
        str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun<int32_t, int32_t>(std::isspace))).base(), str.end());
    }

    void trim (std::string& str) noexcept
    {
        trim_left(str);
        trim_right(str);
    }


    std::vector<std::string> split (const std::string& str, char delimiter) noexcept
    {
        std::vector<std::string> result;
        split(str, delimiter, std::back_inserter(result));
        return result;
    }


    std::string clockToString (const std::chrono::system_clock::time_point& time) noexcept
    {
        time_t currTime = std::chrono::system_clock::to_time_t(time);
        try {
            return std::string(ctime(&currTime)).erase(24, 1).erase(0, 4);
        }
        catch (std::out_of_range& /*err*/) {
            return std::string();
        }
    }


    unsigned char charToUChar (const char symbol) noexcept
    {
        union {
            char in;
            unsigned char out;
        } u { symbol };
        return u.out;
    }

    void replaceNonPrintableToSymbol (void* data, const std::size_t size, const char symbol) noexcept
    {
        auto current = reinterpret_cast<unsigned char*>(data);
        auto end = reinterpret_cast<unsigned char*>(data) + size;
        while (current != end) {
            if (isPrintable(symbol) == true) {
                *current = charToUChar(symbol);
            }
            current++;
        }
    }

}  // namespace common.
