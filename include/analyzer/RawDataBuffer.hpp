#pragma once
#ifndef PROTOCOL_ANALYZER_RAW_DATA_BUFFER_HPP
#define PROTOCOL_ANALYZER_RAW_DATA_BUFFER_HPP

#include <ostream>

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
         * @brief Class that operates on a sequence of bits and offers an interface for working with them.
         *
         * @attention This class MUST BE initialized in all constructors of owner class.
         */
        class BitStreamEngine
        {
            friend class RawDataBuffer;

        private:
            /**
             * @var const RawDataBuffer & storedData;
             * @brief Const reference of the RawDataBuffer owner class.
             */
            const RawDataBuffer & storedData;

        public:
            BitStreamEngine(void) = delete;
            BitStreamEngine (BitStreamEngine &&) = delete;
            BitStreamEngine (const BitStreamEngine &) = delete;
            BitStreamEngine & operator= (BitStreamEngine &&) = delete;
            BitStreamEngine & operator= (const BitStreamEngine &) = delete;

            /**
             * @fn explicit BitStreamEngine::BitStreamEngine (const RawDataBuffer &) noexcept;
             * @brief Constructor of nested BitStreamEngine class.
             * @param [in] owner - Const reference of RawDataBuffer owner class.
             */
            explicit BitStreamEngine (const RawDataBuffer& owner) noexcept
                    : storedData(owner)
            { }

            /**
             * @fn BitStreamEngine::~BitStreamEngine(void) noexcept;
             * @brief Default destructor.
             */
            ~BitStreamEngine(void) noexcept = default;

            /**
             * @fn inline std::size_t BitStreamEngine::Length(void) const noexcept;
             * @brief Method that returns the length of stored data in bits.
             * @return Length of bit sequense of stored data.
             */
            inline std::size_t Length(void) const noexcept { return storedData.Size() * 8; }

            /**
             * @fn const BitStreamEngine & BitStreamEngine::ShiftLeft (std::size_t, bool) const noexcept;
             * @brief Method that performs direct left bit shift by a specified bit offset.
             * @param [in] shift - Bit offset for direct left bit shift.
             * @param [in] fillBit - Value of the fill bit after the left shift. Default: false.
             * @return Const reference of BitStreamEngine class.
             */
            const BitStreamEngine & ShiftLeft (std::size_t /*shift*/, bool /*fillBit*/ = false) const noexcept;

            /**
             * @fn const BitStreamEngine & BitStreamEngine::ShiftRight (std::size_t, bool) const noexcept;
             * @brief Method that performs direct right bit shift by a specified bit offset.
             * @param [in] shift - Bit offset for direct right bit shift.
             * @param [in] fillBit - Value of the fill bit after the right shift. Default: false.
             * @return Const reference of BitStreamEngine class.
             */
            const BitStreamEngine & ShiftRight (std::size_t /*shift*/, bool /*fillBit*/ = false) const noexcept;

            /**
             * @fn const BitStreamEngine & BitStreamEngine::RoundShiftLeft (std::size_t) const noexcept;
             * @brief Method that performs round left bit shift by a specified bit offset.
             * @param [in] shift - Bit offset for round left shift.
             * @return Const reference of BitStreamEngine class.
             */
            const BitStreamEngine & RoundShiftLeft (std::size_t /*shift*/) const noexcept;

            /**
             * @fn const BitStreamEngine & BitStreamEngine::RoundShiftRight (std::size_t) const noexcept;
             * @brief Method that performs round right bit shift by a specified bit offset.
             * @param [in] shift - Bit offset for round right shift.
             * @return Const reference of BitStreamEngine class.
             */
            const BitStreamEngine & RoundShiftRight (std::size_t /*shift*/) const noexcept;

            /**
             * @fn bool BitStreamEngine::Test (std::size_t) const noexcept;
             * @brief Method that checks the bit under the specified index.
             * @param [in] index - Index of bit in binary sequence.
             * @return Boolean value that indicates about the value of the selected bit.
             *
             * @note This method returns value 'false' if the index out-of-range.
             */
            bool Test (std::size_t /*index*/) const noexcept;

            /**
             * @fn inline bool BitStreamEngine::operator[] (const std::size_t) const noexcept;
             * @brief Operator that returns the value of bit under the specified index.
             * @param [in] index - Index of bit in binary sequence.
             * @return Boolean value that indicates about the value of the selected bit.
             */
            inline bool operator[] (const std::size_t index) const noexcept { return Test(index); }

            /**
             * @fn friend std::ostream & operator<< (std::ostream &, const BitStreamEngine &) noexcept;
             * @brief Operator that outputs internal binary raw data in binary string format.
             * @param [in,out] stream - Reference of the output stream engine.
             * @param [in] engine - Reference of the BitStreamEngine class.
             * @return Reference of the inputed STL std::ostream class.
             */
            friend std::ostream & operator<< (std::ostream& stream, const BitStreamEngine& engine) noexcept
            {
                try {
                    stream.unsetf(std::ios_base::boolalpha);
                    for (std::size_t idx = 0; idx < engine.Length(); ++idx) {
                        stream << engine.Test(idx);
                    }
                }
                catch (std::ios_base::failure& /*err*/) { }
                return stream;
            }
        };

    private:
        /**
         * @var std::unique_ptr<std::byte[]> data;
         * @brief Internal variable that contains binary raw data.
         */
        std::unique_ptr<std::byte[]> data = nullptr;
        /**
         * @var std::size_t length;
         * @brief The length of data in bytes.
         */
        std::size_t length = 0;
        /**
         * @var BitStreamEngine bitStreamTransform;
         * @brief Engine for working with sequence of bits.
         */
        BitStreamEngine bitStreamTransform;
        /**
         * @var ByteStreamEngine byteStreamTransform;
         * @brief Engine for working with sequence of bytes.
         */
        //ByteStreamEngine byteStreamTransform;

    public:
        /**
         * @fn RawDataBuffer::RawDataBuffer(void) noexcept;
         * @brief Default constructor.
         */
        RawDataBuffer(void) noexcept
                : bitStreamTransform(*this)
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
         *
         * @attention Need to check the size of data after use this constructor because it is 'noexcept'.
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
         *
         * @attention Need to check the size of data after use this method because it is 'noexcept'.
         */
        explicit RawDataBuffer (std::size_t /*size*/) noexcept;

        /**
         * @fn RawDataBuffer & RawDataBuffer::operator= (const RawDataBuffer &) noexcept;
         * @brief Copy assignment operator.
         * @tparam [in] other - The const reference of copied RawDataBuffer class.
         * @return Reference of the current RawDataBuffer class.
         *
         * @attention Need to check the size of data after use this operator because it is 'noexcept'.
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
         * @return True - if data assignment is successful, otherwise - false.
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
         * @fn template <typename Type>
         * bool RawDataBuffer::AssignData (const Type *, const Type *) noexcept;
         * @brief Method that assigns data to RawDataBuffer.
         * @tparam [in] Type - Typename of assigned data.
         * @param [in] begin - Pointer to the first element of const data of selected type.
         * @param [in] end - Pointer to the element following the last one of const data of selected type.
         * @return True - if data assignment is successful, otherwise - false.
         */
        template <typename Type>
        bool AssignData (const Type* begin, const Type* end) noexcept
        {
            length = std::distance(begin, end) * sizeof(Type);
            data = system::allocMemoryForArray<std::byte>(length, begin, length);
            if (data == nullptr) {
                length = 0;
                return false;
            }
            return true;
        }

        /**
         * @fn const RawDataBuffer::BitStreamEngine & RawDataBuffer::BitsTransform(void) const noexcept;
         * @brief Method that returns const reference of the nested BitStreamEngine class for working with bits.
         * @return Const reference of the BitStreamEngine class.
         */
        const BitStreamEngine& BitsTransform(void) const noexcept { return bitStreamTransform; }

        /**
         * @fn const RawDataBuffer::ByteStreamEngine & RawDataBuffer::BytesTransform(void) const noexcept;
         * @brief Method that returns const reference of the nested ByteStreamEngine class for working with bytes.
         * @return Const reference of the ByteStreamEngine class.
         */
        //const ByteStreamEngine& BytesTransform(void) const noexcept { return byteStreamTransform; }

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
         * @return True - if RawDataBuffer class is empty, otherwise - false.
         */
        inline bool IsEmpty(void) const noexcept { return length == 0; }

        /**
         * @fn explicit operator RawDataBuffer::bool(void) const noexcept;
         * @brief Operator that returns the state of RawDataBuffer.
         * @return True - if RawDataBuffer class is not empty, otherwise - false.
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
