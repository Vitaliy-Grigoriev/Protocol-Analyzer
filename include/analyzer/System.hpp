#pragma once
#ifndef PROTOCOL_ANALYZER_SYSTEM_HPP
#define PROTOCOL_ANALYZER_SYSTEM_HPP

#include <memory>     // std::unique_ptr.
#include <cstring>    // memcpy.
#include <algorithm>  // std::generate.

// In System library MUST NOT use any another library because it is a core library.


namespace analyzer::system
{
    /**
      * @fn template <typename Type, typename... Args>
      * std::unique_ptr<Type> allocMemoryForObject (Args &&...) noexcept;
      * @brief Function that allocates memory for object of selected type and constructs it.
      * @tparam [in] args - Any params for construct object.
      * @return Smart pointer to allocated object of selected type object.
      */
    template <typename Type, typename... Args>
    std::unique_ptr<Type> allocMemoryForObject (Args&&... args) noexcept
    {
        try {
            return std::make_unique<Type>(std::forward<Args>(args)...);
        }
        catch (std::exception& /*err*/) {
            return nullptr;
        }
    }

    /**
      * @fn template <typename Type>
      * std::unique_ptr<Type[]> alloc_memory (const std::size_t, const Type *, const std::size_t) noexcept;
      * @brief Function that allocates memory for array of selected type and if needed fills it.
      * @param [in] size - Size of selected type array.
      * @param [in] data - Pointer to data for copy. Default: nullptr.
      * @param [in] length - Size of data for copy. Default: 0.
      * @return Smart pointer to allocated memory of selected type array.
      */
    template <typename Type = char>
    std::unique_ptr<Type[]> allocMemoryForArray (const std::size_t size, const Type* data = nullptr, const std::size_t length = 0) noexcept
    {
        try {
            auto memory = std::make_unique<Type[]>(size);
            if (data == nullptr) { return memory; }

            if (size <= length) {
                memcpy(memory.get(), data, size * sizeof(Type));
            } else {
                memcpy(memory.get(), data, length * sizeof(Type));
            }
            return memory;
        }
        catch (std::exception& /*err*/) {
            return nullptr;
        }
    }

    /**
      * @fn template <typename Type, typename... Args>
      * std::unique_ptr<std::unique_ptr<Type>[]> allocMemoryForArrayOfObjects (const std::size_t, Args &&...) noexcept;
      * @brief Function that allocates memory for array of selected pointer type and then allocates memory for each object and constructs them.
      * @param [in] size - Size of array of selected smart pointer type.
      * @tparam [in] args - Any params for construct each object.
      * @return Smart pointer to allocated memory of selected smart pointer type array.
      */
    template <typename Type, typename... Args>
    std::unique_ptr<std::unique_ptr<Type>[]> allocMemoryForArrayOfObjects (const std::size_t size, Args&&... args) noexcept
    {
        try {
            auto array = std::make_unique<std::unique_ptr<Type>[]>(size);

            auto construct = [&]() noexcept -> auto { return allocMemoryForObject<Type>(std::forward<Args>(args)...); };
            std::generate(array.get(), array.get() + size, construct);
            return array;
        }
        catch (std::exception& /*err*/) {
            return nullptr;
        }
    }


}  // namespace system.


#endif  // PROTOCOL_ANALYZER_SYSTEM_HPP