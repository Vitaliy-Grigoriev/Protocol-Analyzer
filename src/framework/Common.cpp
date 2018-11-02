// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#include <ctime>  // strftime.
#include <fstream>  // std::ifstream.
#include <stdexcept>  // std::exception, std::out_of_range.
#include <algorithm>  // std::find_if, std::count.

#include "../../include/framework/Common.hpp"


namespace analyzer::framework::common
{
    // Function that converts time point to calendar datetime in string ISO-8601 format.
    std::string clockToString (const std::chrono::system_clock::time_point& time) noexcept
    {
        char buff[22] = { };
        time_t currTime = std::chrono::system_clock::to_time_t(time);
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count() % 1000;
        struct tm* info = std::localtime(&currTime);

        if (std::strftime(buff, sizeof(buff), "%F  %T.", info) > 0) {
            return std::string(buff) + std::to_string(ms);
        }
        return std::string();
    }


    namespace text
    {
        std::string& trimLeft (std::string& str) noexcept
        {
            str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](char symbol) { return !std::isspace(symbol); }));
            return str;
        }

        std::string& trimRight (std::string& str) noexcept
        {
            str.erase(std::find_if(str.rbegin(), str.rend(), [](char symbol) { return !std::isspace(symbol); }).base(), str.end());
            return str;
        }

        std::string& trim (std::string& str) noexcept
        {
            return trimLeft(trimRight(str));
        }


        std::vector<std::string> split (const std::string& str, char delimiter) noexcept
        {
            std::vector<std::string> result;
            split(str, delimiter, std::back_inserter(result));
            return result;
        }

        std::vector<std::string_view> splitInPlace (std::string_view str, char delimiter) noexcept
        {
            std::vector<std::string_view> result;
            std::string_view::size_type start = 0;
            auto pos = str.find_first_of(delimiter, start);
            while (pos != std::string_view::npos)
            {
                if (pos != start) {
                    result.push_back(str.substr(start, pos - start));
                }
                start = pos + 1;
                pos = str.find_first_of(delimiter, start);
            }
            if (start < str.size()) {
                result.push_back(str.substr(start, str.size() - start));
            }
            return result;
        }


        void replaceNonPrintableToSymbol (void* data, const std::size_t size, const char symbol) noexcept
        {
            auto* current = reinterpret_cast<unsigned char*>(data);
            const auto* const end = reinterpret_cast<const unsigned char*>(data) + size;
            while (current != end) {
                if (isPrintable(symbol) == true) {
                    *current = charToUChar(symbol);
                }
                current++;
            }
        }

    }  // namespace text.


    namespace file
    {
        bool checkFileExistence (std::string_view path) noexcept
        {
            std::ifstream file(path.data(), std::ios_base::binary);
            if (file.is_open() == true && file.good() == true)
            {
                file.close();
                return true;
            }
            return false;
        }

        std::size_t getFileSize (std::string_view path) noexcept
        {
            std::ifstream file(path.data(), std::ios_base::binary);
            if (file.is_open() == true && file.good() == true)
            {
                const auto size = file.seekg(0, std::ios_base::end).tellg();
                file.close();
                return static_cast<std::size_t>(size);
            }
            return ErrorState;
        }

        bool readFileToEnd (std::string_view path, std::string& data) noexcept
        {
            try {
                const std::size_t size = getFileSize(path);
                if (size == ErrorState) {
                    return false;
                }
                data.reserve(size);

                std::ifstream file(path.data(), std::ios_base::in);
                if (file.is_open() == true && file.good() == true)
                {
                    data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
                    return true;
                }
            }
            catch (const std::exception& /*err*/) { }
            return false;
        }

        std::size_t getFileLines (std::string_view path) noexcept
        {
            try {
                std::ifstream file(path.data(), std::ios_base::in);
                if (file.is_open() == true && file.good() == true)
                {
                    const auto count = std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n');
                    file.close();
                    return static_cast<std::size_t>(count);
                }
            }
            catch (const std::exception& /*err*/) { }
            return ErrorState;
        }

    }  // namespace file.

}  // namespace common.
