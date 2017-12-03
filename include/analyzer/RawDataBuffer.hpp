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
         * @class BitStreamEngine RawDataBuffer.hpp "include/analyzer/RawDataBuffer.hpp"
         * @brief A class that operates on a sequence of bits and offers an interface for working with them.
         */
        class BitStreamEngine
        {
            friend class RawDataBuffer;

        private:
            /**
             * @var RawDataBuffer & rawData;
             * @brief The reference of RawDataBuffer owner class.
             */
            RawDataBuffer & storedData;

            /**
             * @fn void BitStreamEngine::shiftLeft (std::byte *, std::size_t, std::size_t, bool) const noexcept;
             * @param [in,out] data - Input data sequence for change.
             * @param [in] size - Size of input data in bytes.
             * @param [in] shift - The count of bits for left shift.
             * @param [in] isRound - Boolean flag that indicates about the type of shift: direct or round.
             */
            void shiftLeft (std::byte * /*data*/, std::size_t /*size*/, std::size_t /*shift*/, bool /*isRound*/) const noexcept;

        public:
            /**
             * @fn explicit BitStreamEngine::BitStreamEngine (RawDataBuffer &) noexcept;
             * @brief Default constructor of BitStreamEngine class.
             * @param [in] owner - Reference of RawDataBuffer owner class.
             *
             * @warning This class MUST BE initialized in all constructors of owner class.
             */
            explicit BitStreamEngine (RawDataBuffer& owner) noexcept
                    : storedData(owner)
            { }

            /**
             * @fn inline std::size_t BitStreamEngine::Length(void) const noexcept;
             * @return The length of stored data in bits.
             */
            inline std::size_t Length(void) const noexcept { return storedData.Size() * 8; }

            /**
             * @fn const BitStreamEngine & BitStreamEngine::ShiftLeft (std::size_t) const noexcept;
             * @param [in] shift - The count of bits for left shift.
             * @return The reference of current class.
             */
            const BitStreamEngine & ShiftLeft (std::size_t /*shift*/) const noexcept;

            /**
             * @fn const BitStreamEngine & BitStreamEngine::ShiftRight (std::size_t) const noexcept;
             * @param [in] shift - The count of bits for right shift.
             * @return The reference of current class.
             */
            const BitStreamEngine & ShiftRight (std::size_t /*shift*/) const noexcept;
        };

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
        /**
         * @var BitStreamEngine bitStream;
         * @brief Engine for working with sequence of bits.
         */
        BitStreamEngine bitStream;

    public:
        /**
         * @fn RawDataBuffer::RawDataBuffer(void) noexcept;
         * @brief Default constructor.
         */
        RawDataBuffer(void) noexcept
                : bitStream(*this)
        { }

        /**
         * @fn RawDataBuffer::~RawDataBuffer(void) noexcept;
         * @brief Default destructor.
         */
        ~RawDataBuffer(void) noexcept = default;

        /**
         * @fn RawDataBuffer::RawDataBuffer (const RawDataBuffer &) noexcept;
         * @brief Copy assignment constructor.
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
         * @brief Copy assignment operator.
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
         * @return Size of stored data in bytes.
         */
        inline std::size_t Size(void) const noexcept { return length; }

        /**
         * @fn inline const std::byte * RawDataBuffer::Data(void) const noexcept;
         * @brief Method that returns the pointer to the const internal data.
         * @return Pointer to the const internal data.
         */
        inline const std::byte* Data(void) const noexcept { return data.get(); }

        /**
         * @fn inline bool RawDataBuffer::IsEmpty(void) const noexcept;
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
         * @brief Operator that returns a const reference to an element by selected index.
         * @param [in] index - Index of element in data.
         * @return Return a const reference to an element by selected index.
         *
         * @attention Undefined behavior error may occur when using this method (Out-Of-Range).
         */
        inline const std::byte& operator[] (const std::size_t index) const noexcept { return data[index]; }

        /**
         * @fn std::byte * RawDataBuffer::GetAt (std::size_t) const noexcept;
         * @brief Method that returns a pointer to an element by selected index.
         * @param [in] index - Index of element in data.
         * @return Return a pointer to an element by selected index or nullptr in an error occurred.
         */
        std::byte* GetAt (std::size_t /*index*/) const noexcept;

        /**
         * @fn void RawDataBuffer::Clear(void) noexcept;
         * @brief Method that clears the data.
         */
        void Clear(void) noexcept;

        /**
         * @fn bool RawDataBuffer::CheckSystemEndian(void) const noexcept;
         * @brief Method that checks the endian on the system.
         * @return True - if on the system little endian, false - big endian.
         */
        bool CheckSystemEndian(void) const noexcept;
    };

}  // namespace types.


#endif  // PROTOCOL_ANALYZER_RAW_DATA_BUFFER_HPP
