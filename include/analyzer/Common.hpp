#pragma once
#ifndef PROTOCOL_ANALYZER_COMMON_HPP
#define PROTOCOL_ANALYZER_COMMON_HPP

#include <limits>
#include <random>
#include <memory>
#include <chrono>
#include <sstream>
#include <cstring>
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
      * @brief Function, which allocate dynamic memory.
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
      * @brief Function that allocate dynamic memory and if needed fills it.
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
     * @brief Function that return the sequence of pseudo-random integral numbers.
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
     * @fn std::string clockToString (const std::chrono::system_clock::time_point &) noexcept;
     * @brief Function that convert time point to calendar datetime in string format.
     * @param [in] time - The time point in processes clock ticks.
     * @return Calendar datetime in string format.
     */
    std::string clockToString (const std::chrono::system_clock::time_point & /*time*/) noexcept;


    template <typename T = char>
    struct data_container
    {
        std::unique_ptr<T[]> data = nullptr;
        std::size_t length = 0;

        data_container (T* in, const std::size_t size) noexcept
                : data(in), length(size)
        { }
        data_container (std::unique_ptr<T[]>& in, const std::size_t size) noexcept
                : data(std::move(in)), length(size)
        { }
    };

    //const std::regex FindAllHrefs("href\\s*=\\s*[\"']([^\"']+)[\"']", std::regex::icase);
    //const std::regex FindApacheStruts("href\\s*=\\s*[\"']([^\"']+\\.action)[\"']", std::regex::icase);
    //const std::regex FindHTTPHeaderFields("^(([^ ]+)?\\s*:([^\\r\\n]+)?\\r\\n)?");


}  // namespace common.

#endif  // PROTOCOL_ANALYZER_COMMON_HPP
