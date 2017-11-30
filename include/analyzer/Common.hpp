#pragma once
#ifndef PROTOCOL_ANALYZER_COMMON_HPP
#define PROTOCOL_ANALYZER_COMMON_HPP

#include <limits>
#include <random>
#include <chrono>
#include <string>
#include <iomanip>
#include <sstream>
#include <cstddef> // std::size_t, std::byte.
#include <cstdint> // std::int*_t.
#include <iterator>
#include <exception>
#include <algorithm>
#include <string_view> // std::string_view.
#include <type_traits>

// In Common library MUST NOT use any another library because it is a core library.

#define DEFAULT_BUFFER_SIZE 1048576  // 1 Mb.


namespace analyzer::common
{
    /**
     * @fn template <typename Type>
     * std::enable_if_t<std::is_integral<Type>::value, Type>
     * GetRandomValue(void) noexcept;
     * @brief Function that returns the sequence of pseudo-random integral numbers.
     * @tparam [in] Type - Typename of the integral value for the random generating engine. Default: uint32_t.
     * @return The pseudo-random number of the selected type.
     */
    template <typename Type = uint32_t>
    std::enable_if_t<std::is_integral<Type>::value, Type>
    GetRandomValue(void) noexcept
    {
        using param_t = typename std::uniform_int_distribution<Type>::param_type;
        std::mt19937 gen{ std::random_device()() };
        std::uniform_int_distribution<Type> dist;

        return dist(gen, param_t(std::numeric_limits<Type>::min() + 1, std::numeric_limits<Type>::max() - 1));
    }


    /**
     * @fn std::string clockToString (const std::chrono::system_clock::time_point &) noexcept;
     * @brief Function that converts time point to calendar datetime in string format.
     * @param [in] time - The time point in processes clock ticks.
     * @return Calendar datetime in string format.
     */
    std::string clockToString (const std::chrono::system_clock::time_point & /*time*/) noexcept;


    namespace text
    {
        /**
         * @fn std::string & trimLeft (std::string &) noexcept;
         * @brief Trim string value from start in place.
         * @param [in,out] str - Reference of target string.
         * @return Reference of the trimmed string.
         */
        std::string & trimLeft (std::string & /*str*/) noexcept;

        /**
         * @fn std::string & trimRight (std::string &) noexcept;
         * @brief Trim string value from end in place.
         * @param [in,out] str - Reference of target string.
         * @return Reference of the trimmed string.
         */
        std::string & trimRight (std::string & /*str*/) noexcept;

        /**
         * @fn std::string & trim (std::string &) noexcept;
         * @brief Trim string value from both ends in place.
         * @param [in,out] str - Reference of target string.
         * @return Reference of the trimmed string.
         */
        std::string & trim (std::string & /*str*/) noexcept;


        /**
         * @fn static inline std::string trimLeftCopy (std::string) noexcept;
         * @brief Copying trim string value from start.
         * @param [in] str - Target string.
         * @return Trimmed string.
         */
        static inline std::string trimLeftCopy (std::string str) noexcept {
            return trimLeft(str);;
        }

        /**
         * @fn static inline std::string trimRightCopy (std::string) noexcept;
         * @brief Copying trim string value from end.
         * @param [in] str - Target string.
         * @return Trimmed string.
         */
        static inline std::string trimRightCopy (std::string str) noexcept {
            return trimRight(str);
        }

        /**
         * @fn static inline std::string trimCopy (std::string) noexcept;
         * @brief Copying trim string value from both sides.
         * @param [in] str - Target string.
         * @return Trimmed string from both sides.
         */
        static inline std::string trimCopy (std::string str) noexcept {
            return trim(str);
        }


        /**
         * @fn template <typename Out>
         * void split (const std::string &, char, std::insert_iterator<Out>) noexcept;
         * @brief Split string into a vector of strings using the delimiter.
         * @param [in] str - Input string.
         * @param [in] delimiter - Parsing separator.
         * @tparam [out] result - Back insert iterator of selected type for adding new values.
         */
        template <typename Out>
        void split (const std::string& str, char delimiter, std::back_insert_iterator<Out> result) noexcept
        {
            std::istringstream ss(str);
            std::string item;
            while (std::getline(ss, item, delimiter)) {
                *(result++) = item;
            }
        }

        /**
         * @fn std::vector<std::string> split (const std::string &, char) noexcept;
         * @brief Split string into a vector of strings using the delimiter.
         * @param [in] str - Input string.
         * @param [in] delimiter - Parsing separator.
         * @return The vector of string values.
         */
        std::vector<std::string> split (const std::string & /*str*/, char /*delimiter*/) noexcept;

        /**
         * @fn std::vector<std::string_view> splitInPlace (std::string_view, char) noexcept;
         * @brief Unallocated split string into a vector of strings using the delimiter.
         * @param [in] str - Unallocated input string reference.
         * @param [in] delimiter - Parsing separator.
         * @return The vector of unallocated string values.
         */
        std::vector<std::string_view> splitInPlace (std::string_view /*str*/, char /*delimiter*/) noexcept;


        /**
         * @fn template <typename Type>
         * std::string getHexValue (const Type &, const uint16_t, bool) noexcept;
         * @brief Function that converts any data to hex format.
         * @tparam [in] data - Input data of selected type.
         * @param [in] width - Width of hex value. Default: 2.
         * @param [in] upper - In what case the data will present in hex format. Default: true.
         * @return String in hex format of width length.
         */
        template <typename Type,
                typename std::enable_if<std::is_integral<Type>::value>::type * = nullptr,
                typename std::enable_if<std::is_unsigned<Type>::value>::type * = nullptr>
        std::string getHexValue (const Type& data, const uint16_t width = 2, bool upper = true) noexcept
        {
            std::ostringstream result;
            if (upper == true) { result.setf(std::ios_base::uppercase); }
            result << std::hex << std::setfill('0') << std::setw(width) << static_cast<std::size_t>(data);
            return result.str();
        }

        /**
         * @fn template <typename Type>
         * std::string getHexString (const Type *, const std::size_t, const uint16_t, bool) noexcept;
         * @brief Function that converts string data to hex format.
         * @tparam [in] data - Input data of selected type.
         * @param [in] length - Length of the data.
         * @param [in] width - Width of hex value for each type. Default: 2.
         * @param [in] upper - In what case the data will present in hex format. Default: true.
         * @return String in hex format.
         */
        template <typename Type>
        std::string getHexString (const Type* data, const std::size_t length, const uint16_t width = 2, bool upper = true) noexcept
        {
            std::string result;
            result.reserve(length * width);
            for (std::size_t idx = 0; idx < length; ++idx) {
                result += getHexValue(data[idx], width * sizeof(Type), upper);
            }
            return result;
        }

        /**
         * @fn unsigned char charToUChar (char) noexcept;
         * @brief Function that converts one value of char type to unsigned char type.
         * @param [in] symbol - One value in char type.
         * @return One value in unsigned char type.
         */
        unsigned char charToUChar (char /*symbol*/) noexcept;

        /**
         * @fn static inline bool isPrintable (const char) noexcept;
         * @brief Function that checks the char value on printable.
         * @param [in] symbol - One value in char type.
         * @return True - if character is printable, otherwise - false.
         */
        static inline bool isPrintable (const char symbol) noexcept {
            return (symbol >= 0x20 && symbol < 0x7f);
        }

        /**
         * @fn static inline bool isNumber (const char) noexcept;
         * @brief Function that checks the char value on number.
         * @param [in] symbol - One value in char type.
         * @return True - if character is number, otherwise - false.
         */
        static inline bool isNumber (const char symbol) noexcept {
            return (symbol >= 0x30 && symbol <= 0x39);
        }

        /**
         * @fn void replaceNonPrintableToSymbol (void *, std::size_t, char) noexcept;
         * @brief Function that replaces all non-printable values to symbol.
         * @param [in,out] data - Pointer to input data.
         * @param [in] size - Size of input data.
         * @param [in] symbol - One value in char type for insert. Default: '.' (0x2E).
         */
        void replaceNonPrintableToSymbol (void * data, std::size_t size, char symbol = '.') noexcept;

    }  // namespace text.


    namespace file
    {
        /**
         * @var static const std::size_t file_error;
         * @brief Define the error code of file functions.
         */
        static const std::size_t ErrorState = std::numeric_limits<std::size_t>::max();

        /**
         * @fn bool checkFileExistence (const std::string_view &) noexcept;
         * @brief Function that checks file existence.
         * @param [in] path - Path to file.
         * @return True - if file is exist, otherwise - false.
         */
        bool checkFileExistence (std::string_view /*path*/) noexcept;

        /**
         * @fn std::size_t getFileSize (const std::string_view &) noexcept;
         * @brief Function that returns file size.
         * @param [in] path - Path to file.
         * @return File size in bytes or maximum value of std::size_t if an error.
         */
        std::size_t getFileSize (std::string_view /*path*/) noexcept;

        /**
         * @fn bool readFileToEnd (const std::string_view &, std::string &) noexcept;
         * @brief Function that reads all data from file.
         * @param [in] path - Path to file.
         * @param [out] data - String value for assign.
         * @return True - if the data is read from the file successfully, otherwise - false.
         */
        bool readFileToEnd (std::string_view /*path*/, std::string & /*data*/) noexcept;

        /**
         * @fn std::size_t getFileLines (const std::string_view &) noexcept;
         * @brief Function that returns number of file lines.
         * @param [in] path - Path to file.
         * @return Number of file lines or maximum value of std::size_t if an error.
         */
        std::size_t getFileLines (std::string_view /*path*/) noexcept;

    }  // namespace file.


    namespace convert
    {
        /**
         * @fn template <std::size_t I, std::size_t J, typename Type>
         * auto toPair (const Type &) -> decltype(std::make_pair(std::get<I>(...), std::get<J>(...)))
         * @brief Function that converts two indexes container value to pair type in compile time.
         * @param [in] I - Index of the first element in input container.
         * @param [in] J - Index of the second element in input container.
         * @tparam [in] Type - Container type (for example: tuple);
         * @tparam [in] value - Input container of selected type.
         * @return The pair type of two selected elements in container.
         */
        template <std::size_t I, std::size_t J, typename Type>
        auto toPair (const Type& value) -> decltype(std::make_pair(std::get<I>(value), std::get<J>(value)))
        {
            return std::make_pair(std::get<I>(value), std::get<J>(value));
        }

    }  // namespace convert.



    /**
     * @class Data Common.hpp "include/analyzer/Common.hpp"
     * @brief Class container that defined the interface to manage anything data.
     * @tparam [in] Type - Typename of stored data. Default: char.
     */
    template <typename Type = char>
    class Data
    {
    private:
        /**
         * @var std::unique_ptr<Type[]> data;
         * @brief Variable that contains unique pointer to data.
         */
        std::unique_ptr<Type[]> data = nullptr;
        /**
         * @var std::size_t length;
         * @brief Variable that contains length of stored data.
         */
        std::size_t length = 0;

    public:
        /**
         * @fn Data::Data (Type *, const std::size_t) noexcept;
         * @brief Constructor of Data class.
         * @tparam [in] in - Any data for sharing.
         * @param [in] size - Size of this data.
         */
        Data (Type* in, const std::size_t size) noexcept
                : data(in), length(size)
        { }

        /**
         * @fn Data::Data (std::unique_ptr<Type[]> &, const std::size_t) noexcept;
         * @brief Constructor of Data class.
         * @tparam [in] in - Any data for sharing.
         * @param [in] size - Size of this data.
         */
        Data (std::unique_ptr<Type[]>& in, const std::size_t size) noexcept
                : data(std::move(in)), length(size)
        { }

        /**
         * @fn inline std::size_t Data::Size(void) const noexcept;
         * @brief Method that returns size of the internal data.
         * @return Size of the internal data.
         */
        inline std::size_t Size(void) const noexcept
        {
            return length;
        }

        /**
         * @fn inline Type * Data::Get(void) const noexcept;
         * @brief Method that returns pointer to the internal data.
         * @return Pointer to the internal data.
         */
        inline Type* Get(void) const noexcept
        {
            return data.get();
        }

        /**
         * @fn inline Type * Data::GetAt (const std::size_t) const noexcept;
         * @brief Method that returns a pointer to an element by selected index.
         * @param [in] index - Index of element in data.
         * @return Return a pointer to an element by selected index or nullptr in an error occurred.
         */
        inline Type* GetAt (const std::size_t index) const noexcept
        {
            return index < length ? &data[index] : nullptr;
        }
    };

    //const std::regex FindAllHrefs("href\\s*=\\s*[\"']([^\"']+)[\"']", std::regex::icase);
    //const std::regex FindApacheStruts("href\\s*=\\s*[\"']([^\"']+\\.action)[\"']", std::regex::icase);
    //const std::regex FindHTTPHeaderFields("^(([^ ]+)?\\s*:([^\\r\\n]+)?\\r\\n)?");


}  // namespace common.


#endif  // PROTOCOL_ANALYZER_COMMON_HPP
