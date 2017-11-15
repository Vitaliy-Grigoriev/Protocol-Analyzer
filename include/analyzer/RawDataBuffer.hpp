#pragma once
#ifndef PROTOCOL_ANALYZER_RAW_DATA_BUFFER_HPP
#define PROTOCOL_ANALYZER_RAW_DATA_BUFFER_HPP

// In RawDataBuffer library MUST NOT use any another library because it is a core library.


namespace analyzer::common::types
{
    /**
     * @class RawDataBuffer RawDataBuffer.hpp "include/analyzer/RawDataBuffer.hpp"
     * @brief Main class of analyzer framework that contains binary data and give an interface to work with it.
     */
    class RawDataBuffer
    {
    private:
        /**
         * @var std::unique_ptr<std::byte[]>data;
         * @brief Internal variable that contains binary raw data.
         */
        std::unique_ptr<std::byte[]> data = nullptr;
        /**
         * @var std::size_t length;
         * @brief The length of data in bytes.
         */
        std::size_t length = 0;

    public:
        /**
         * @fn RawDataBuffer::RawDataBuffer(void);
         * @brief Default constructor.
         */
        RawDataBuffer(void) = default;

        /**
         * @fn RawDataBuffer::~RawDataBuffer(void);
         * @brief Default destructor.
         */
        ~RawDataBuffer(void) = default;

        /**
         * @fn RawDataBuffer::RawDataBuffer (const RawDataBuffer &) noexcept;
         * @brief Copy constructor.
         * @tparam [in] other - The const reference of copied RawDataBuffer class.
         */
        RawDataBuffer (const RawDataBuffer & /*other*/) noexcept;

        /**
         * @fn RawDataBuffer::RawDataBuffer (const RawDataBuffer &&) noexcept;
         * @brief Move assignment constructor.
         * @tparam [in] other - The rvalue reference of moved RawDataBuffer class.
         */
        RawDataBuffer (RawDataBuffer && /*other*/) noexcept;

        /**
         * @fn explicit RawDataBuffer::RawDataBuffer (std::size_t) noexcept;
         * @brief Constructor that allocates specified amount of bytes.
         * @param [in] size - Number of bytes for allocate.
         */
        explicit RawDataBuffer (std::size_t /*size*/) noexcept;

        /**
         * @fn RawDataBuffer & RawDataBuffer::operator= (const RawDataBuffer &) noexcept;
         * @brief Copy operator.
         * @tparam [in] other - The const reference of copied RawDataBuffer class.
         * @return Reference of the current RawDataBuffer class.
         */
        RawDataBuffer & operator= (const RawDataBuffer & /*other*/) noexcept;

        /**
         * @fn RawDataBuffer & RawDataBuffer::operator= (RawDataBuffer &&) noexcept;
         * @brief Move assignment operator.
         * @tparam [in] other - The rvalue reference of moved RawDataBuffer class.
         * @return Reference of the current RawDataBuffer class.
         */
        RawDataBuffer & operator= (RawDataBuffer && /*other*/) noexcept;

        /**
         * @fn template <typename Type>
         * bool RawDataBuffer::AssignData (const Type *, const std::size_t) noexcept;
         * @brief Method that assigns data to RawDataBuffer.
         * @tparam [in] Type - Typename of assigned data.
         * @param [in] other - Pointer to the const data of selected type.
         * @param [in] size - The number of elements in the input data of selected type.
         * @return True - if assignment is successful, otherwise - false.
         */
        template <typename Type>
        bool AssignData (const Type* other, const std::size_t size) noexcept
        {
            length = size * sizeof(Type);
            data = system::allocMemoryForArray<std::byte>(length, other, length);
            if (data == nullptr) {
                length = 0;
                return false;
            }
            return true;
        }

        /**
         * @fn inline std::size_t RawDataBuffer::Size(void) const noexcept
         * @brief Method that returns the size of data.
         * @return Size of internal data in bytes.
         */
        inline std::size_t Size(void) const noexcept { return length; }

        /**
         * @fn inline const std::byte * RawDataBuffer::Data(void) const noexcept
         * @brief Method that returns the pointer to the const internal data.
         * @return Pointer to the const internal data.
         */
        inline const std::byte* Data(void) const noexcept { return data.get(); }

        /**
         * @fn inline bool RawDataBuffer::IsEmpty(void) const noexcept
         * @brief Method that returns the state of RawDataBuffer.
         * @return True - if RawDataBuffer is empty, otherwise - false.
         */
        inline bool IsEmpty(void) const noexcept { return length == 0; }

        /**
         * @fn explicit operator RawDataBuffer::bool(void) const noexcept;
         * @brief Operator that returns the state of RawDataBuffer.
         * @return True - if RawDataBuffer is not empty, otherwise - false.
         */
        explicit operator bool(void) const noexcept { return length != 0; }

        /**
         * @fn inline const std::byte & RawDataBuffer::operator[] (std::size_t) const noexcept;
         * @brief Operator that returns the const reference to the element under the specified index.
         * @param [in] index - Index of element in data.
         * @return Return the const reference to the element under the specified index.
         */
        inline const std::byte& operator[] (const std::size_t index) const noexcept { return data[index]; }


        /**
         * @fn void Clear(void) noexcept;
         * @brief Method that clears the data.
         */
        void Clear(void) noexcept;
    };

}  // namespace types.


#endif  // PROTOCOL_ANALYZER_RAW_DATA_BUFFER_HPP
