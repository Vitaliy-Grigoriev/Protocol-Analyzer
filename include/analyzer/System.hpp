// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef PROTOCOL_ANALYZER_SYSTEM_HPP
#define PROTOCOL_ANALYZER_SYSTEM_HPP

#include <memory>   // std::unique_ptr<>().
#include <cstring>  // memcpy().

// In System library must not use any another library because it is a core library.


namespace analyzer::system
{
    /**
      * @fn template <typename T> std::unique_ptr<T> alloc_memory(void) noexcept;
      * @brief Function that allocates dynamic memory.
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
      * @fn template <typename T = char[]> std::unique_ptr<T[]> alloc_memory (const std::size_t, const T *) noexcept;
      * @brief Function that allocates dynamic memory and if needed fills it.
      * @param [in] size - The size of memory.
      * @param [in] data - The pointer to data for copy. Default: nullptr.
      * @param [in] length - The length of data for copy.
      * @return The smart pointer to allocated memory.
      */
    template <typename T = char>
    std::unique_ptr<T[]> alloc_memory (const std::size_t size, const T* data = nullptr, const std::size_t length = 0) noexcept
    {
        try {
            auto memory = std::make_unique<T[]>(size);
            if (data == nullptr) { return memory; }

            if (size <= length) {
                memcpy(memory.get(), data, size);
            } else {
                memcpy(memory.get(), data, length);
            }
            return memory;
        }
        catch (std::exception& /*err*/) {
            return nullptr;
        }
    }

}  // namespace system.


#endif  // PROTOCOL_ANALYZER_SYSTEM_HPP
