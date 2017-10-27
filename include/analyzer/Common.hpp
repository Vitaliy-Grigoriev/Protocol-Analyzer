#pragma once
#ifndef PROTOCOL_ANALYZER_COMMON_HPP
#define PROTOCOL_ANALYZER_COMMON_HPP

#include <limits>
#include <random>
#include <memory>
#include <chrono>
#include <string>
#include <iomanip>
#include <cstring>
#include <sstream>
#include <cstdint>
#include <iterator>
#include <exception>
#include <algorithm>
#include <type_traits>

// In Common library must not use any another library because it is a core library.

#define DEFAULT_BUFFER_SIZE 1048576  // 1 Mb.

#define DEFAULT_TIMEOUT_TASK_THREAD 10  // sec.


namespace analyzer::common
{
    /**
      * @fn template <typename T> std::unique_ptr<T> alloc_memory(void) noexcept;
      * @brief Function, which allocates dynamic memory.
      * @return The smart pointer to allocated memory.
      */
    template <typename T>
    std::unique_ptr<T> alloc_memory(void) noexcept
    {
        try {
            return std::make_unique<T>();
        }
        catch (std::exception& /*err*/) {
            return nullptr;
        }
    }

    /**
      * @fn template <typename T = char[]> std::unique_ptr<T[]> alloc_memory (const std::size_t, const T * = nullptr) noexcept;
      * @brief Function that allocates dynamic memory and if needed fills it.
      * @param [in] size - The size of memory.
      * @param [in] data - The pointer to data for copy.
      * @return The smart pointer to allocated memory.
      */
    template <typename T = char>
    std::unique_ptr<T[]> alloc_memory (const std::size_t size, const T * data = nullptr) noexcept
    {
        try {
            auto mem = std::make_unique<T[]>(size);
            if (data == nullptr) { return mem; }
            memcpy(mem.get(), data, size);
            return mem;
        }
        catch (std::exception& /*err*/) {
            return nullptr;
        }
    }


    /**
     * @fn template <typename T = uint32_t> std::enable_if_t<std::is_integral<T>::value, T> GetRandomValue(void) noexcept;
     * @brief Function that returns the sequence of pseudo-random integral numbers.
     * @return The pseudo-random number of the type T.
     */
    template <typename T = uint32_t>
    std::enable_if_t<std::is_integral<T>::value, T>
    GetRandomValue(void) noexcept
    {
        using param_t = typename std::uniform_int_distribution<T>::param_type;
        std::mt19937 gen{ std::random_device()() };
        std::uniform_int_distribution<T> dist;

        return dist(gen, param_t(std::numeric_limits<T>::min() + 1, std::numeric_limits<T>::max() - 1));
    }


    namespace text
    {
        /**
         * @fn void trim_left (std::string &);
         * @brief Trim string value from start in place.
         * @param [in,out] str - Target string.
         */
        void trim_left (std::string & /*str*/) noexcept;

        /**
         * @fn void trim_right (std::string &);
         * @brief Trim string value from end in place.
         * @param [in,out] str - Target string.
         */
        void trim_right (std::string & /*str*/) noexcept;


        /**
         * @fn void trim (std::string &);
         * @brief Trim string value from both ends in place.
         * @param [in,out] str - Target string.
         */
        void trim (std::string & /*str*/) noexcept;

        /**
         * @fn template<typename Out> void split (const std::string &, char, std::insert_iterator<Out>) noexcept;
         * @brief Split string into a vector of strings using the delimiter.
         * @param [in] str - Input string.
         * @param [in] delimiter - Parsing separator.
         * @param [in,out] result - Back iterator for adding new values.
         */
        template<typename Out>
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
         * @fn template<typename Type> std::string getHexValue (const Type &, const uint16_t, bool) noexcept;
         * @brief Function that converts any data to hex format.
         * @param [in] data - Input data.
         * @param [in] width - Width of hex value. Default: 2.
         * @param [in] upper - In what case the data will present in hex format. Default: true.
         * @return String in hex format of width length.
         */
        template<typename Type,
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
         * @fn template<typename Type> std::string getHexString (const Type *, const std::size_t, const uint16_t, bool) noexcept;
         * @brief Function that convertss string data to hex format.
         * @param [in] data - Input data.
         * @param [in] length - Length of the data.
         * @param [in] width - Width of hex value for each type. Default: 2.
         * @param [in] upper - In what case the data will present in hex format. Default: true.
         * @return String in hex format.
         */
        template<typename Type>
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
         * @fn bool checkFileExistence (const std::string & ) noexcept;
         * @brief Function that checks file existence.
         * @param [in] path - Path to file.
         * @return True - if file is exist, otherwise - false.
         */
        bool checkFileExistence (const std::string & /*path*/) noexcept;

        /**
         * @fn std::size_t getFileSize (const std::string & ) noexcept;
         * @brief Function that returns file size.
         * @param [in] path - Path to file.
         * @return File size in bytes or maximum value of std::size_t if an error.
         */
        std::size_t getFileSize (const std::string & /*path*/) noexcept;

        /**
         * @fn bool readFileToEnd (const std::string &, std::string &) noexcept;
         * @brief Function that reads all data from file.
         * @param [in] path - Path to file.
         * @param [out] data - String value for assign.
         * @return True - if the data is read from the file successfully, otherwise - false.
         */
        bool readFileToEnd (const std::string & /*path*/, std::string & /*data*/) noexcept;

        /**
         * @fn std::size_t getFileLines (const std::string & ) noexcept;
         * @brief Function that returns number of file lines.
         * @param [in] path - Path to file.
         * @return Number of file lines or maximum value of std::size_t if an error.
         */
        std::size_t getFileLines (const std::string & /*path*/) noexcept;
    }  // namespace file.

    /**
     * @fn std::string clockToString (const std::chrono::system_clock::time_point &) noexcept;
     * @brief Function that converts time point to calendar datetime in string format.
     * @param [in] time - The time point in processes clock ticks.
     * @return Calendar datetime in string format.
     */
    std::string clockToString (const std::chrono::system_clock::time_point & /*time*/) noexcept;


    /**
     * @class Data Common.hpp "include/analyzer/Common.hpp"
     * @brief Class container that defined the interface to manage anything data.
     */
    template <typename T = char>
    class Data
    {
    private:
        std::unique_ptr<T[]> data = nullptr;
        std::size_t length = 0;

    public:
        /**
         * @fn Data::Data (T *, const std::size_t) noexcept;
         * @brief Constructor of Data class.
         * @tparam [in] in - Any data for sharing.
         * @param [in] size - Size of this data.
         */
        Data (T* in, const std::size_t size) noexcept
                : data(in), length(size)
        { }

        /**
         * @fn Data::Data (std::unique_ptr<T[]> &, const std::size_t) noexcept;
         * @brief Constructor of Data class.
         * @tparam [in] in - Any data for sharing.
         * @param [in] size - Size of this data.
         */
        Data (std::unique_ptr<T[]>& in, const std::size_t size) noexcept
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
         * @fn inline T * Data::Get(void) const noexcept;
         * @brief Method that returns pointer to the internal data.
         * @return Pointer to the internal data.
         */
        inline T* Get(void) const noexcept
        {
            return data.get();
        }

        /**
         * @fn inline T * Data::GetAt (const std::size_t) const noexcept;
         * @brief Method that returns pointer to any shift of the internal data.
         * @return Pointer to any shift of the internal data.
         */
        inline T* GetAt (const std::size_t index) const noexcept
        {
            return index < length ? &data[index] : nullptr;
        }
    };

    //const std::regex FindAllHrefs("href\\s*=\\s*[\"']([^\"']+)[\"']", std::regex::icase);
    //const std::regex FindApacheStruts("href\\s*=\\s*[\"']([^\"']+\\.action)[\"']", std::regex::icase);
    //const std::regex FindHTTPHeaderFields("^(([^ ]+)?\\s*:([^\\r\\n]+)?\\r\\n)?");


}  // namespace common.

#endif  // PROTOCOL_ANALYZER_COMMON_HPP
