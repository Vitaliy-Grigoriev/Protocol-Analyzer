// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#ifndef PROTOCOL_ANALYZER_SYSTEM_HPP
#define PROTOCOL_ANALYZER_SYSTEM_HPP

#include <memory>  // std::unique_ptr, std::make_unique.
#include <utility>  // std::forward
#include <cstring>  // memcpy, memset.
#include <algorithm>  // std::generate.

// In System library MUST NOT use any another functional framework libraries because it is a core library.


namespace analyzer::framework::system
{
    /**
      * @fn template <typename Type, typename... Args>
      * std::unique_ptr<Type> allocMemoryForObject (Args &&...) noexcept;
      * @brief Function that allocates memory for object of selected type and constructs it.
      * @tparam [in] Type - Typename of allocated data.
      * @tparam [in] args - Arguments for construct the object.
      * @return Smart pointer to allocated object of selected type object.
      *
      * @note Return value is marked with the "nodiscard" attribute.
      * @attention All exceptions that can be thrown from the object constructor must be inherited from STL std::exception.
      */
    template <typename Type, typename... Args>
    [[nodiscard]]
    std::unique_ptr<Type> allocMemoryForObject (Args&&... args) noexcept
    {
        try {
            return std::make_unique<Type>(std::forward<Args>(args)...);
        }
        catch (const std::exception& /*err*/) {
            return nullptr;
        }
    }

    /**
      * @fn template <typename Type>
      * std::unique_ptr<Type[]> allocMemoryForArray (const std::size_t, const void *, const std::size_t) noexcept;
      * @brief Function that allocates memory for array of selected type and if needed fills it.
      * @tparam [in] Type - Typename of allocated data.
      * @param [in] count - The number of elements of selected type.
      * @param [in] data - Pointer to any data for copy. Default: nullptr.
      * @param [in] length - Size of data for copy in bytes. Default: 0.
      * @return Smart pointer to allocated memory of selected type array.
      *
      * @note Return value is marked with the "nodiscard" attribute.
      */
    template <typename Type>
    [[nodiscard]]
    std::unique_ptr<Type[]> allocMemoryForArray (const std::size_t count, const void* data = nullptr, const std::size_t length = 0) noexcept
    {
        try
        {
            auto memory = std::make_unique<Type[]>(count);
            if (data == nullptr || length == 0) { return memory; }

            const std::size_t allocatedBytes = count * sizeof(Type);
            if (allocatedBytes <= length) {
                memcpy(memory.get(), data, allocatedBytes);
            }
            else {  // If copied data length less then allocated data size.
                memcpy(memory.get(), data, length);
                memset(memory.get() + length, 0, allocatedBytes - length);
            }
            return memory;
        }
        catch (const std::exception& /*err*/) {
            return nullptr;
        }
    }

    /**
      * @fn template <typename Type, typename... Args>
      * std::unique_ptr<std::unique_ptr<Type>[]> allocMemoryForArrayOfObjects (const std::size_t, Args &&...) noexcept;
      * @brief Function that allocates memory for array of selected pointer type and then allocates memory for each object and constructs them.
      * @tparam [in] Type - Typename of allocated data.
      * @param [in] size - Size of array of selected smart pointer type.
      * @tparam [in] args - Arguments for construct each object in array.
      * @return Smart pointer to allocated memory of selected smart pointer type array.
      *
      * @note Return value is marked with the "nodiscard" attribute.
      * @attention All exceptions that can be thrown from the object constructor must be inherited from STL std::exception.
      */
    template <typename Type, typename... Args>
    [[nodiscard]]
    std::unique_ptr<std::unique_ptr<Type>[]> allocMemoryForArrayOfObjects (const std::size_t size, Args&&... args) noexcept
    {
        try {
            auto array = std::make_unique<std::unique_ptr<Type>[]>(size);

            auto construct = [&]() noexcept { return std::make_unique<Type>(std::forward<Args>(args)...); };
            std::generate(array.get(), array.get() + size, construct);
            return array;
        }
        catch (const std::exception& /*err*/) {
            return nullptr;
        }
    }

}  // namespace system.


#endif  // PROTOCOL_ANALYZER_SYSTEM_HPP
