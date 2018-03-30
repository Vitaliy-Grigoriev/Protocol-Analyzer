// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#pragma once
#ifndef PROTOCOL_ANALYZER_RAW_DATA_BUFFER_HPP
#define PROTOCOL_ANALYZER_RAW_DATA_BUFFER_HPP

#include <map>  // std::pair.
#include <string>  // std::string.
#include <ostream>  // std::ostream.
#include <iterator>  // std::iterator_traits.
#include <type_traits>  // std::enable_if, std::is_same, std::is_trivial, std::is_standard_layout.

//#include "RawDataBufferIterator.hpp"

// In Common library MUST NOT use any another framework libraries because it is a core library.

////////////// DATA ENDIAN TYPE //////////////
//
//  Little Endian Model.
//  |7______0|15______8|23______16|31______24|
//
//  Middle Endian Model. (PDP-11)
//  |23______16|31______24|7______0|15______8|
//
//  Big Endian Model.
//  |31______24|23______16|15______8|7______0|
//
//  Endian Independent Model.
//  |0______7|8______15|16______23|24______31|
//
//////////////////////////////////////////////


namespace analyzer::common::types
{
    /**
     * @enum DATA_ENDIAN_TYPE
     * @brief Endian type of the stored data in RawDataBuffer class.
     *
     * @note Default initial type of internal data in RawDataBuffer class the same as system type.
     */
    enum DATA_ENDIAN_TYPE : uint16_t
    {
        DATA_LITTLE_ENDIAN = 0x01,  // Last byte of binary representation of the multibyte data-type is stored first.
        DATA_BIG_ENDIAN = 0x02      // First byte of binary representation of the multibyte data-type is stored first.
    };

    /**
     * @enum DATA_HANDLING_MODE
     * @brief Type of the data handling mode in RawDataBuffer class.
     *
     * @note Default initial type in RawDataBuffer class is DATA_MODE_DEPENDENT and DATA_MODE_SAFE_OPERATOR.
     * @note Data handling mode DATA_MODE_INDEPENDENT needs specially for analyze unstructured data.
     * @note Changed only the method of processing data. Data representation does not changes after change the handling mode.
     */
    enum DATA_HANDLING_MODE : uint16_t
    {
        DATA_MODE_DEPENDENT = 0x01,        // Any data modification depends on the ending type.
        DATA_MODE_INDEPENDENT = 0x02,      // Any data modification does not depend on the ending type.
        DATA_MODE_SAFE_OPERATOR = 0x04,    // Any data modification by assign logical operators does not lead to a change the data length.
        DATA_MODE_UNSAFE_OPERATOR = 0x08,  // Any data modification by assign logical operators leads to a change the data length.
    };


    /**
     * @class RawDataBuffer RawDataBuffer.hpp "include/analyzer/RawDataBuffer.hpp"
     * @brief Main class of analyzer framework that contains binary raw data and gives an interface to work with it.
     */
    class RawDataBuffer
    {
        //friend class RawDataBufferIterator<8>;
        //friend class RawDataBufferConstIterator<RawDataBuffer>;

    public:
        /**
         * @var static const std::size_t npos;
         * @brief Variable that indicates about the end of sequence.
         */
        static const std::size_t npos = static_cast<std::size_t>(-1);

        using value_type = std::byte;

        //using iterator = RawDataBufferIterator<RawDataBuffer>;
        //using reverse_iterator = std::reverse_iterator<iterator>;

        //using const_iterator = RawDataBufferConstIterator<RawDataBuffer>;
        //using const_reverse_iterator = std::reverse_iterator<const_iterator>;

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
             * @brief Const lvalue reference of the RawDataBuffer owner class.
             */
            const RawDataBuffer & storedData;

            /**
             * @fn std::pair<std::size_t, std::byte> GetBitPosition (std::size_t) const noexcept;
             * @brief Method that returns the correct position of selected bit in stored raw data in any data endian.
             * @param [in] index - Index of bit in binary sequence.
             * @return Index of element in array of raw stored data and shift to this bit in selected part.
             *
             * @note Method returns index 'npos' if selected index is out of range.
             */
            std::pair<std::size_t, std::byte> GetBitPosition (std::size_t /*index*/) const noexcept;

        public:
            BitStreamEngine(void) = delete;
            BitStreamEngine (BitStreamEngine &&) = delete;
            BitStreamEngine (const BitStreamEngine &) = delete;
            BitStreamEngine & operator= (BitStreamEngine &&) = delete;
            BitStreamEngine & operator= (const BitStreamEngine &) = delete;

            /**
             * @fn explicit BitStreamEngine::BitStreamEngine (const RawDataBuffer &) noexcept;
             * @brief Constructor of nested BitStreamEngine class.
             * @param [in] owner - Const lvalue reference of RawDataBuffer owner class.
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
             * @return Length of bit sequence of stored data.
             */
            inline std::size_t Length(void) const noexcept { return storedData.length * 8; }

            /**
             * @fn const BitStreamEngine & BitStreamEngine::ShiftLeft (std::size_t, bool) const noexcept;
             * @brief Method that performs direct left bit shift by a specified bit offset.
             * @param [in] shift - Bit offset for direct left bit shift.
             * @param [in] fillBit - Value of the fill bit after the left shift. Default: false (0).
             * @return Const lvalue reference of BitStreamEngine class.
             */
            const BitStreamEngine & ShiftLeft (std::size_t /*shift*/, bool /*fillBit*/ = false) const noexcept;

            /**
             * @fn const BitStreamEngine & BitStreamEngine::ShiftRight (std::size_t, bool) const noexcept;
             * @brief Method that performs direct right bit shift by a specified bit offset.
             * @param [in] shift - Bit offset for direct right bit shift.
             * @param [in] fillBit - Value of the fill bit after the right shift. Default: false (0).
             * @return Const lvalue reference of BitStreamEngine class.
             */
            const BitStreamEngine & ShiftRight (std::size_t /*shift*/, bool /*fillBit*/ = false) const noexcept;

            /**
             * @fn const BitStreamEngine & BitStreamEngine::RoundShiftLeft (std::size_t) const noexcept;
             * @brief Method that performs round left bit shift by a specified bit offset.
             * @param [in] shift - Bit offset for round left bit shift.
             * @return Const lvalue reference of BitStreamEngine class.
             */
            const BitStreamEngine & RoundShiftLeft (std::size_t /*shift*/) const noexcept;

            /**
             * @fn const BitStreamEngine & BitStreamEngine::RoundShiftRight (std::size_t) const noexcept;
             * @brief Method that performs round right bit shift by a specified bit offset.
             * @param [in] shift - Bit offset for round right bit shift.
             * @return Const lvalue reference of BitStreamEngine class.
             */
            const BitStreamEngine & RoundShiftRight (std::size_t /*shift*/) const noexcept;

            /**
             * @fn bool BitStreamEngine::Test (std::size_t) const noexcept;
             * @brief Method that checks the bit under the specified index.
             * @param [in] index - Index of bit in binary sequence.
             * @return Boolean value that indicates about the value of the selected bit.
             *
             * @warning Method always returns value 'false' if the index out-of-range.
             */
            bool Test (std::size_t /*index*/) const noexcept;

            /**
             * @fn bool BitStreamEngine::All (std::size_t, std::size_t) const noexcept;
             * @brief Method that returns bit sequence characteristic when all bits are set in block of stored data.
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be checked. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be checked. Default: npos.
             * @return True - if all bits in block of stored data are set, otherwise - false.
             */
            bool All (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos) const noexcept;

            /**
             * @fn bool BitStreamEngine::Any (std::size_t, std::size_t) const noexcept;
             * @brief Method that returns bit sequence characteristic when any of the bits are set in block of stored data.
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be checked. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be checked. Default: npos.
             * @return True - if any of the bits in block of stored data are set, otherwise - false.
             */
            bool Any (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos) const noexcept;

            /**
             * @fn bool BitStreamEngine::None (std::size_t, std::size_t) const noexcept;
             * @brief Method that returns bit sequence characteristic when none of the bits are set in block of stored data.
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be checked. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be checked. Default: npos.
             * @return True - if none of the bits in block of stored data are set, otherwise - false.
             */
            bool None (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos) const noexcept;

            /**
             * @fn std::size_t BitStreamEngine::Count (std::size_t, std::size_t) const noexcept;
             * @brief Method that returns the number of bits that are set in the selected interval of stored data.
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be checked. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be checked. Default: npos.
             * @return Number of bits that are set in block of stored data.
             *
             * @note Method returns RawDataBuffer::npos value if an error occurred.
             */
            std::size_t Count (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos) const noexcept;

            /**
             * @fn std::size_t BitStreamEngine::GetFirstIndex (std::size_t, std::size_t) const noexcept;
             * @brief Method that returns position of the first set bit in the selected interval of stored data.
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be checked. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be checked. Default: npos.
             * @param [in] isRelative - Boolean flag that indicates about the type of return index. Default: true.
             * @return Position of the first set bit in the selected interval of stored data.
             *
             * @note Method returns RawDataBuffer::npos value if there are no set bits on the specified interval.
             * @note Method returns RawDataBuffer::npos value if an error occurred.
             */
            std::size_t GetFirstIndex (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos, bool isRelative = true) const noexcept;

            /**
             * @fn std::size_t BitStreamEngine::GetLastIndex (std::size_t, std::size_t) const noexcept;
             * @brief Method that returns position of the last set bit in the selected interval of stored data.
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be checked. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be checked. Default: npos.
             * @param [in] isRelative - Boolean flag that indicates about the type of return index. Default: true.
             * @return Position of the last set bit in the selected interval of stored data.
             *
             * @note Method returns RawDataBuffer::npos value if there are no set bits on the specified interval.
             * @note Method returns RawDataBuffer::npos value if an error occurred.
             */
            std::size_t GetLastIndex (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos, bool isRelative = true) const noexcept;

            /**
             * @fn const BitStreamEngine & BitStreamEngine::Set (std::size_t, bool) const noexcept;
             * @brief Method that sets the bit under the specified index to new value.
             * @param [in] index - Index of bit in binary sequence.
             * @param [in] fillBit - New value of selected bit. Default: true (1).
             * @return Const lvalue reference of BitStreamEngine class.
             */
            const BitStreamEngine & Set (std::size_t /*index*/, bool /*fillBit*/ = true) const noexcept;

            /**
             * @fn const BitStreamEngine & BitStreamEngine::Reverse (std::size_t, std::size_t) const noexcept;
             * @brief Method that reverses a sequence of bits under the specified first/last indexes.
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be reversed. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be reversed. Default: npos.
             * @return Const lvalue reference of BitStreamEngine class.
             */
            const BitStreamEngine & Reverse (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos) const noexcept;

            /**
             * @fn const BitStreamEngine & BitStreamEngine::Invert (std::size_t) const noexcept;
             * @brief Method that inverts the bit under the specified index.
             * @param [in] index - Index of bit in binary sequence.
             * @return Const lvalue reference of BitStreamEngine class.
             */
            const BitStreamEngine & Invert (std::size_t /*index*/) const noexcept;

            /**
             * @fn const BitStreamEngine & BitStreamEngine::InvertBlock (std::size_t, std::size_t) const noexcept;
             * @brief Method that inverts the range of bits under the specified first/last indexes.
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be inverted. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be inverted. Default: npos.
             * @return Const lvalue reference of BitStreamEngine class.
             */
            const BitStreamEngine & InvertBlock (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos) const noexcept;

            /**
             * @fn std::string BitStreamEngine::ToString (std::size_t, std::size_t) const noexcept;
             * @brief Method that outputs internal binary raw data in string format.
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be outputed. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be outputed. Default: npos.
             * @return STL string object with sequence of the bit character representation of stored data.
             *
             * @note Data is always outputs in DATA_BIG_ENDIAN endian type if data handling mode is DATA_MODE_DEPENDENT.
             */
            std::string ToString (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos) const noexcept;

            /**
             * @fn std::string BitStreamEngine::ToString (std::size_t, std::size_t) const noexcept;
             * @brief Method that outputs internal binary raw data in short string format.
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be outputted. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be outputted. Default: npos.
             * @param [in] compression - Bit sequence compression level. Default: 1.
             * @return STL string object with sequence of the bit character representation of stored data in short format.
             *
             * @note Data is always outputs in DATA_BIG_ENDIAN endian type if data handling mode is DATA_MODE_DEPENDENT.
             * @note Use this function only for large bit sequences.
             * @note Example: 10010011  --1-->  1(0){2}1(0){2}(1){2}  --2-->  (1(0){2}){2}(1){2}.
             */
            std::string ToShortString (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos, uint16_t /*compression*/ = 1) const noexcept;

            /**
             * @fn inline bool BitStreamEngine::operator[] (const std::size_t) const noexcept;
             * @brief Operator that returns the value of bit under the specified index.
             * @param [in] index - Index of bit in binary sequence.
             * @return Value of the selected bit.
             */
            inline bool operator[] (const std::size_t index) const noexcept { return Test(index); }

            /**
             * @fn friend inline std::ostream & operator<< (std::ostream &, const BitStreamEngine &) noexcept;
             * @brief Operator that outputs internal binary raw data in binary string format.
             * @param [in,out] stream - Reference of the output stream engine.
             * @param [in] engine - Const lvalue reference of the BitStreamEngine class.
             * @return Lvalue reference of the inputted STL std::ostream class.
             *
             * @note Data is always outputs in DATA_BIG_ENDIAN endian type if data handling mode is DATA_MODE_DEPENDENT.
             */
            friend inline std::ostream& operator<< (std::ostream& stream, const BitStreamEngine& engine) noexcept
            {
                try
                {
                    if (engine.storedData.IsEmpty() == false)
                    {
                        stream.unsetf(std::ios_base::boolalpha);
                        if ((engine.storedData.DataModeType() & DATA_MODE_DEPENDENT) != 0u)
                        {
                            for (std::size_t idx = engine.Length() - 1; idx != 0; --idx) {
                                stream << engine.Test(idx);
                            }
                            stream << engine.Test(0);
                        }
                        else  // If data handling mode is DATA_MODE_INDEPENDENT.
                        {
                            for (std::size_t idx = 0; idx < engine.Length(); ++idx) {
                                stream << engine.Test(idx);
                            }
                        }
                    }
                }
                catch (const std::ios_base::failure& /*err*/) { }
                return stream;
            }


            /**
             * @fn const BitStreamEngine & BitStreamEngine::operator<<= (std::size_t) const noexcept;
             * @brief Bitwise left shift assignment operator that performs direct left bit shift by a specified bit offset.
             * @param [in] shift - Bit offset for direct left bit shift as right operand.
             * @return Const lvalue reference of transformed BitStreamEngine class.
             */
            const BitStreamEngine & operator<<= (std::size_t /*shift*/) const noexcept;

            /**
             * @fn const BitStreamEngine & BitStreamEngine::operator>>= (std::size_t) const noexcept;
             * @brief Bitwise right shift assignment operator that performs direct right bit shift by a specified bit offset.
             * @param [in] shift - Bit offset for direct right bit shift as right operand.
             * @return Const lvalue reference of transformed BitStreamEngine class.
             */
            const BitStreamEngine & operator>>= (std::size_t /*shift*/) const noexcept;

            /**
             * @fn const BitStreamEngine & BitStreamEngine::operator&= (const BitStreamEngine &) const noexcept;
             * @brief Logical assignment bitwise AND operator that transforms internal binary raw data.
             * @param [in] other - Const lvalue reference of the BitStreamEngine class as right operand.
             * @return Const lvalue reference of transformed (by operation AND) BitStreamEngine class.
             *
             * @note If operands have different raw data length then result data will be the length of the lesser among the operands.
             * @note If data handling mode is DATA_MODE_SAFE_OPERATOR then the size of result data will be preserved.
             *
             * @attention If operands have different endian depended handling mode then no changes will be made.
             */
            const BitStreamEngine & operator&= (const BitStreamEngine & /*other*/) const noexcept;

            /**
             * @fn const BitStreamEngine & BitStreamEngine::operator|= (const BitStreamEngine &) const noexcept;
             * @brief Logical assignment bitwise OR operator that transforms internal binary raw data.
             * @param [in] other - Const lvalue reference of the BitStreamEngine class as right operand.
             * @return Const lvalue reference of transformed (by operation OR) BitStreamEngine class.
             *
             * @note If operands have different raw data length then result data will be the length of the largest among the operands.
             * @note If data handling mode is DATA_MODE_SAFE_OPERATOR then the size of result data will be preserved.
             *
             * @attention If operands have different endian depended handling mode then no changes will be made.
             */
            const BitStreamEngine & operator|= (const BitStreamEngine & /*other*/) const noexcept;

            /**
             * @fn const BitStreamEngine & BitStreamEngine::operator^= (const BitStreamEngine &) const noexcept;
             * @brief Logical assignment bitwise XOR operator that transforms internal binary raw data.
             * @param [in] other - Const lvalue reference of the BitStreamEngine class as right operand.
             * @return Const lvalue reference of transformed (by operation XOR) BitStreamEngine class.
             *
             * @note If operands have different raw data length then result data will be the length of the largest among the operands.
             * @note If data handling mode is DATA_MODE_SAFE_OPERATOR then the size of result data will be preserved.
             *
             * @attention If operands have different endian depended handling mode then no changes will be made.
             */
            const BitStreamEngine & operator^= (const BitStreamEngine & /*other*/) const noexcept;

            /**
             * @fn friend inline RawDataBuffer BitStreamEngine::operator& (const BitStreamEngine &, const BitStreamEngine &) noexcept;
             * @brief Logical bitwise AND operator that transforms internal binary raw data.
             * @param [in] left - Const lvalue reference of the BitStreamEngine class as left operand.
             * @param [in] right - Const lvalue reference of the BitStreamEngine class as right operand.
             * @return New temporary object of transformed (by operation AND) RawDataBuffer class.
             *
             * @note If operands have different endian and mode then result RawDataBuffer will have endian and mode of the left operand.
             * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LESSER length.
             *
             * @attention Result RawDataBuffer class always processing in DATA_MODE_UNSAFE_OPERATOR mode.
             */
            friend inline RawDataBuffer operator& (const BitStreamEngine& left, const BitStreamEngine& right) noexcept
            {
                RawDataBuffer result(left.storedData);
                result.SetDataModeType(types::DATA_MODE_UNSAFE_OPERATOR);
                if (result.IsEmpty() == false) {
                    result.BitsTransform() &= right;
                }
                result.SetDataModeType(left.storedData.DataModeType());
                return result;
            }

            /**
             * @fn friend inline RawDataBuffer BitStreamEngine::operator| (const BitStreamEngine &, const BitStreamEngine &) noexcept;
             * @brief Logical bitwise OR operator that transforms internal binary raw data.
             * @param [in] left - Const lvalue reference of the BitStreamEngine class as left operand.
             * @param [in] right - Const lvalue reference of the BitStreamEngine class as right operand.
             * @return New temporary object of transformed (by operation OR) RawDataBuffer class.
             *
             * @note If operands have different endian and mode then result RawDataBuffer will have endian and mode of the left operand.
             * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LONGER length.
             *
             * @attention Result RawDataBuffer class always processing in DATA_MODE_UNSAFE_OPERATOR mode.
             */
            friend inline RawDataBuffer operator| (const BitStreamEngine& left, const BitStreamEngine& right) noexcept
            {
                RawDataBuffer result(left.storedData);
                result.SetDataModeType(types::DATA_MODE_UNSAFE_OPERATOR);
                if (result.IsEmpty() == false) {
                    result.BitsTransform() |= right;
                }
                result.SetDataModeType(left.storedData.DataModeType());
                return result;
            }

            /**
             * @fn friend inline RawDataBuffer BitStreamEngine::operator^ (const BitStreamEngine &, const BitStreamEngine &) noexcept;
             * @brief Logical bitwise XOR operator that transforms internal binary raw data.
             * @param [in] left - Const lvalue reference of the BitStreamEngine class as left operand.
             * @param [in] right - Const lvalue reference of the BitStreamEngine class as right operand.
             * @return New temporary object of transformed (by operation XOR) RawDataBuffer class.
             *
             * @note If operands have different endian and mode then result RawDataBuffer will have endian and mode of the left operand.
             * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LONGER length.
             *
             * @attention Result RawDataBuffer class always processing in DATA_MODE_UNSAFE_OPERATOR mode.
             */
            friend inline RawDataBuffer operator^ (const BitStreamEngine& left, const BitStreamEngine& right) noexcept
            {
                RawDataBuffer result(left.storedData);
                result.SetDataModeType(types::DATA_MODE_UNSAFE_OPERATOR);
                if (result.IsEmpty() == false) {
                    result.BitsTransform() ^= right;
                }
                result.SetDataModeType(left.storedData.DataModeType());
                return result;
            }

            /**
             * @fn friend inline RawDataBuffer BitStreamEngine::operator<< (const BitStreamEngine &, const std::size_t) noexcept;
             * @brief Bitwise left shift operator that performs direct left bit shift by a specified bit offset.
             * @param [in] engine - Const lvalue reference of the BitStreamEngine class as left operand.
             * @param [in] shift - Bit offset for direct left bit shift as right operand.
             * @return New temporary object of transformed RawDataBuffer class.
             */
            friend inline RawDataBuffer operator<< (const BitStreamEngine& engine, const std::size_t shift) noexcept
            {
                RawDataBuffer result(engine.storedData);
                result.BitsTransform().ShiftLeft(shift, false);
                return result;
            }

            /**
             * @fn friend inline RawDataBuffer BitStreamEngine::operator>> (const BitStreamEngine &, const std::size_t) noexcept;
             * @brief Bitwise right shift operator that performs direct right bit shift by a specified bit offset.
             * @param [in] engine - Const lvalue reference of the BitStreamEngine class as left operand.
             * @param [in] shift - Bit offset for direct right bit shift as right operand.
             * @return New temporary object of transformed RawDataBuffer class.
             */
            friend inline RawDataBuffer operator>> (const BitStreamEngine& engine, const std::size_t shift) noexcept
            {
                RawDataBuffer result(engine.storedData);
                result.BitsTransform().ShiftRight(shift, false);
                return result;
            }

            /**
             * @fn friend inline RawDataBuffer BitStreamEngine::operator~ (const BitStreamEngine &) const noexcept;
             * @brief Logical bitwise complement operator that inverts each bit in internal binary raw data.
             * @param [in] engine - Const lvalue reference of the BitStreamEngine class.
             * @return New temporary object of transformed (by operation NOT) RawDataBuffer class.
             */
            friend inline RawDataBuffer operator~ (const BitStreamEngine& engine) noexcept
            {
                RawDataBuffer result(engine.storedData);
                result.BitsTransform().InvertBlock();
                return result;
            }

        };


        /**
         * @class ByteStreamEngine RawDataBuffer.hpp "include/analyzer/RawDataBuffer.hpp"
         * @brief Class that operates on a sequence of bytes and offers an interface for working with them.
         *
         * @attention This class MUST BE initialized in all constructors of owner class.
         */
        class ByteStreamEngine
        {
            friend class RawDataBuffer;

        private:
            /**
             * @var const RawDataBuffer & storedData;
             * @brief Const lvalue reference of the RawDataBuffer owner class.
             */
            const RawDataBuffer & storedData;

            /**
             * @fn std::size_t GetBytePosition (std::size_t) const noexcept;
             * @brief Method that returns the correct position of selected byte in stored raw data in any data endian.
             * @param [in] index - Index of byte in byte sequence.
             * @return Index of element in array of raw stored data.
             *
             * @note Method returns index 'npos' if selected index is out of range.
             */
            std::size_t GetBytePosition (std::size_t /*index*/) const noexcept;

        public:
            ByteStreamEngine(void) = delete;
            ByteStreamEngine (ByteStreamEngine &&) = delete;
            ByteStreamEngine (const ByteStreamEngine &) = delete;
            ByteStreamEngine & operator= (ByteStreamEngine &&) = delete;
            ByteStreamEngine & operator= (const ByteStreamEngine &) = delete;

            /**
             * @fn explicit ByteStreamEngine::ByteStreamEngine (const RawDataBuffer &) noexcept;
             * @brief Constructor of nested ByteStreamEngine class.
             * @param [in] owner - Const lvalue reference of RawDataBuffer owner class.
             */
            explicit ByteStreamEngine (const RawDataBuffer& owner) noexcept
                    : storedData(owner)
            { }

            /**
             * @fn ByteStreamEngine::~ByteStreamEngine(void) noexcept;
             * @brief Default destructor.
             */
            ~ByteStreamEngine(void) noexcept = default;

            /**
             * @fn inline std::size_t ByteStreamEngine::Length(void) const noexcept;
             * @brief Method that returns the length of stored data in bytes.
             * @return Length of byte sequence of stored data.
             */
            inline std::size_t Length(void) const noexcept { return storedData.length; }

            /**
             * @fn const ByteStreamEngine & ByteStreamEngine::ShiftLeft (std::size_t, std::byte) const noexcept;
             * @brief Method that performs direct left byte shift by a specified byte offset.
             * @param [in] shift - Byte offset for direct left byte shift.
             * @param [in] fillByte - Value of the fill byte after the left shift. Default: 0x00.
             * @return Const lvalue reference of ByteStreamEngine class.
             */
            const ByteStreamEngine & ShiftLeft (std::size_t /*shift*/, std::byte /*fillByte*/ = std::byte(0x00)) const noexcept;

            /**
             * @fn const ByteStreamEngine & ByteStreamEngine::ShiftRight (std::size_t, std::byte) const noexcept;
             * @brief Method that performs direct right byte shift by a specified byte offset.
             * @param [in] shift - Byte offset for direct right byte shift.
             * @param [in] fillByte - Value of the fill byte after the right shift. Default: 0x00.
             * @return Const lvalue reference of ByteStreamEngine class.
             */
            const ByteStreamEngine & ShiftRight (std::size_t /*shift*/, std::byte /*fillByte*/ = std::byte(0x00)) const noexcept;

            /**
             * @fn const ByteStreamEngine & ByteStreamEngine::RoundShiftLeft (std::size_t) const noexcept;
             * @brief Method that performs round left bit shift by a specified byte offset.
             * @param [in] shift - Byte offset for round left byte shift.
             * @return Const lvalue reference of ByteStreamEngine class.
             */
            const ByteStreamEngine & RoundShiftLeft (std::size_t /*shift*/) const noexcept;

            /**
             * @fn const ByteStreamEngine & ByteStreamEngine::RoundShiftRight (std::size_t) const noexcept;
             * @brief Method that performs round right bit shift by a specified byte offset.
             * @param [in] shift - Byte offset for round right byte shift.
             * @return Const lvalue reference of ByteStreamEngine class.
             */
            const ByteStreamEngine & RoundShiftRight (std::size_t /*shift*/) const noexcept;

            /**
             * @fn inline std::byte ByteStreamEngine::operator[] (const std::size_t) const noexcept;
             * @brief Operator that returns the value of byte under the specified index.
             * @param [in] index - Index of byte in byte sequence.
             * @return Value of the selected byte.
             *
             * @note Method returns '0x00' if selected index is out of range.
             */
            inline std::byte operator[] (const std::size_t index) const noexcept
            {
                if (index >= Length()) { return std::byte(0x00); }
                return storedData.data[GetBytePosition(index)];
            }

            /**
             * @fn std::byte * ByteStreamEngine::GetAt (std::size_t) const noexcept;
             * @brief Method that returns a pointer to the value of byte under the specified index.
             * @param [in] index - Index of byte in byte sequence.
             * @return Return a pointer to the value of byte under the specified index or nullptr in an error occurred.
             */
            std::byte * GetAt (std::size_t /*index*/) const noexcept;
        };

    private:
        /**
         * @var mutable std::unique_ptr<std::byte[]> data;
         * @brief Internal variable that contains binary raw data.
         */
        mutable std::unique_ptr<std::byte[]> data = nullptr;
        /**
         * @var mutable std::size_t length;
         * @brief Length of stored data in bytes.
         */
        mutable std::size_t length = 0;
        /**
         * @var DATA_HANDLING_MODE dataMode;
         * @brief Type of the data handling mode.
         */
        uint16_t dataMode = DATA_MODE_DEPENDENT | DATA_MODE_SAFE_OPERATOR;
        /**
         * @var DATA_ENDIAN_TYPE dataEndian;
         * @brief Endian of stored data.
         */
        uint16_t dataEndian = DATA_LITTLE_ENDIAN;
        /**
         * @var BitStreamEngine bitStreamTransform;
         * @brief Engine for working with sequence of bits.
         */
        BitStreamEngine bitStreamTransform;
        /**
         * @var ByteStreamEngine byteStreamTransform;
         * @brief Engine for working with sequence of bytes.
         */
        ByteStreamEngine byteStreamTransform;


        template<typename Type, typename = void>
        struct is_iterator_type
        {
            static constexpr bool value = false;
        };

        template<typename Type>
        struct is_iterator_type<Type, typename std::enable_if_t<!std::is_same<typename std::iterator_traits<Type>::value_type, void>::value>>
        {
            static constexpr bool value = true;
        };

        template<typename Type, typename = void>
        struct is_pod_type
        {
            static constexpr bool value = false;
        };

        template<typename Type>
        struct is_pod_type<Type, typename std::enable_if_t<std::is_trivial<Type>::value && std::is_standard_layout<Type>::value>>
        {
            static constexpr bool value = true;
        };

    public:
        /**
         * @fn explicit RawDataBuffer::RawDataBuffer (DATA_HANDLING_MODE, DATA_ENDIAN_TYPE) noexcept;
         * @param [in] mode - Type of the data handling mode. Default: DATA_MODE_DEPENDENT | DATA_MODE_SAFE_OPERATOR.
         * @param [in] endian - Endian of stored data. Default: Local System Type.
         * @brief Default constructor.
         */
        explicit RawDataBuffer (uint16_t mode = DATA_MODE_DEPENDENT | DATA_MODE_SAFE_OPERATOR,
                                uint16_t endian = CheckSystemEndian()) noexcept
                : dataMode(mode), dataEndian(endian), bitStreamTransform(*this), byteStreamTransform(*this)
        { }

        /**
         * @fn RawDataBuffer::~RawDataBuffer(void) noexcept;
         * @brief Default destructor.
         */
        ~RawDataBuffer(void) noexcept = default;

        /**
         * @fn RawDataBuffer::RawDataBuffer (const RawDataBuffer &) noexcept;
         * @brief Copy assignment constructor.
         * @tparam [in] other - Const lvalue reference of copied RawDataBuffer class.
         *
         * @attention Need to check the size of data after use this constructor because it is 'noexcept'.
         */
        RawDataBuffer (const RawDataBuffer & /*other*/) noexcept;

        /**
         * @fn RawDataBuffer::RawDataBuffer (const RawDataBuffer &&) noexcept;
         * @brief Move assignment constructor.
         * @tparam [in] other - Rvalue reference of moved RawDataBuffer class.
         */
        RawDataBuffer (RawDataBuffer && /*other*/) noexcept;

        /**
         * @fn explicit RawDataBuffer::RawDataBuffer (std::size_t, DATA_HANDLING_MODE, DATA_ENDIAN_TYPE) noexcept;
         * @brief Constructor that allocates specified amount of bytes.
         * @param [in] size - Number of bytes for allocate.
         * @param [in] mode - Type of the data handling mode. Default: DATA_MODE_DEPENDENT | DATA_MODE_SAFE_OPERATOR.
         * @param [in] endian - Endian of stored data. Default: Local System Type.
         *
         * @attention Need to check the size of data after use this method because it is 'noexcept'.
         */
        explicit RawDataBuffer (std::size_t /*size*/,
                                uint16_t mode = DATA_MODE_DEPENDENT | DATA_MODE_SAFE_OPERATOR,
                                uint16_t endian = CheckSystemEndian()) noexcept;

        /**
         * @fn RawDataBuffer & RawDataBuffer::operator= (const RawDataBuffer &) noexcept;
         * @brief Copy assignment operator.
         * @tparam [in] other - Const lvalue reference of copied RawDataBuffer class.
         * @return Reference of the current RawDataBuffer class.
         *
         * @attention Need to check the size of data after use this operator because it is 'noexcept'.
         */
        RawDataBuffer & operator= (const RawDataBuffer & /*other*/) noexcept;

        /**
         * @fn RawDataBuffer & RawDataBuffer::operator= (RawDataBuffer &&) noexcept;
         * @brief Move assignment operator.
         * @tparam [in] other - Rvalue reference of moved RawDataBuffer class.
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
         *
         * @note Input type MUST be a POD type.
         */
        template <typename Type>
        bool AssignData (const Type* other, const std::size_t size) noexcept
        {
            static_assert(is_pod_type<Type>::value == true, "It is not possible to use not POD type for this function.");

            if (other == nullptr) { return false; }

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
         * bool RawDataBuffer::AssignData (const Type, const Type) noexcept;
         * @brief Method that assigns data to RawDataBuffer.
         * @tparam [in] Type - Typename of assigned data.
         * @param [in] begin - Iterator to the first element of const data of selected type.
         * @param [in] end - Iterator to the element following the last one of const data of selected type.
         * @return True - if data assignment is successful, otherwise - false.
         *
         * @note Input type under iterator MUST be a POD type.
         */
        template <typename Type>
        bool AssignData (const Type begin, const Type end) noexcept
        {
            static_assert(is_iterator_type<Type>::value == true &&
                                  is_pod_type<typename std::iterator_traits<Type>::value_type>::value == true,
                          "It is not possible to use not Iterator type for this function.");

            length = std::distance(begin, end) * sizeof(std::iterator_traits<Type>::value_type);
            data = system::allocMemoryForArray<std::byte>(length, &(*begin), length);
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
        const ByteStreamEngine& BytesTransform(void) const noexcept { return byteStreamTransform; }

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
         * @brief Method that returns the state of RawDataBuffer class.
         * @return True - if stored data is empty in RawDataBuffer class, otherwise - false.
         */
        inline bool IsEmpty(void) const noexcept { return length == 0; }

        /**
         * @fn inline DATA_HANDLING_MODE RawDataBuffer::DataModeType(void) const noexcept;
         * @brief Method that returns the data handling mode type of stored data in RawDataBuffer class.
         * @return DATA_MODE_DEPENDENT(0x01) - if any data modification depends on the endian type, otherwise - DATA_MODE_INDEPENDENT(0x08).
         */
        inline uint16_t DataModeType(void) const noexcept { return dataMode; }

        /**
         * @fn inline DATA_ENDIAN_TYPE RawDataBuffer::DataEndianType(void) const noexcept;
         * @brief Method that returns the endian type of stored data in RawDataBuffer class.
         * @return DATA_LITTLE_ENDIAN(0x01) - if on the system little endian, otherwise - DATA_BIG_ENDIAN(0x02).
         */
        inline uint16_t DataEndianType(void) const noexcept { return dataEndian; }

        /**
         * @fn void RawDataBuffer::SetDataModeType (DATA_HANDLING_MODE) noexcept;
         * @brief Method that changes handling mode type of stored data in RawDataBuffer class.
         * @param [in] mode - New data handling mode type.
         *
         * @note Changed only the method of processing data. Data representation does not changes.
         * @note After setting a new mode then the opposite mode will be automatically turned off.
         */
        void SetDataModeType (uint16_t /*mode*/) noexcept;

        /**
         * @fn void RawDataBuffer::SetDataEndianType (DATA_ENDIAN_TYPE, bool) noexcept;
         * @brief Method that changes endian type of stored data in RawDataBuffer class.
         * @param [in] endian - New data endian type.
         * @param [in] convert - Flag indicating whether to change the presentation of the stored data or not. Default: true.
         */
        void SetDataEndianType (uint16_t /*endian*/, bool /*convert*/ = true) noexcept;

        /**
         * @fn explicit operator RawDataBuffer::bool(void) const noexcept;
         * @brief Operator that returns the internal state of RawDataBuffer class.
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
        std::byte * GetAt (std::size_t /*index*/) const noexcept;

        /**
         * @fn void RawDataBuffer::Clear(void) noexcept;
         * @brief Method that clears the data.
         */
        void Clear(void) noexcept;

        /**
         * @fn DATA_ENDIAN_TYPE RawDataBuffer::CheckSystemEndian(void) noexcept;
         * @brief Method that checks the endian type on the system.
         * @return DATA_LITTLE_ENDIAN(0x01) - if on the system little endian, otherwise - DATA_BIG_ENDIAN(0x02).
         */
        inline static DATA_ENDIAN_TYPE CheckSystemEndian(void) noexcept
        {
            union {
                uint16_t first;
                uint8_t second[2];
            } type { 0x0102 };
            return (type.second[0] == 0x02 ? DATA_LITTLE_ENDIAN : DATA_BIG_ENDIAN);
        }

    };


}  // namespace types.


#endif  // PROTOCOL_ANALYZER_RAW_DATA_BUFFER_HPP
