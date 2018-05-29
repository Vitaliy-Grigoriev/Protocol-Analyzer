// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#pragma once
#ifndef PROTOCOL_ANALYZER_RAW_DATA_BUFFER_HPP
#define PROTOCOL_ANALYZER_RAW_DATA_BUFFER_HPP

#include <map>  // std::pair.
#include <ostream>  // std::ostream.

#include "System.hpp"  // system::allocMemoryForArray.
#include "Common.hpp"  // common::is_pod_type, common::is_iterator_type.

//#include "BinaryDataEngineIterator.hpp"

// In Common library MUST NOT use any another functional framework libraries because it is a core library.

//////////////// DATA ENDIAN TYPE ////////////////
//
//  Little Endian Model.
//  |7______0|15______8|23______16|31______24|
//
//  Big Endian Model.
//  |31______24|23______16|15______8|7______0|
//
//  Endian Independent Model.
//  |0______7|8______15|16______23|24______31|
//
//  Another endian models.
//
//  Middle Endian Big Model. (PDP-11)
//  |23______16|31______24|7______0|15______8|
//
//  Middle Endian Little Model. (Honeywell 316)
//  |15______8|7______0|31______24|23______16|
//
//////////////////////////////////////////////////


namespace analyzer::common::types
{
    /**
     * @enum DATA_ENDIAN_TYPE
     * @brief Endian type of the stored data in BinaryDataEngine class.
     *
     * @note Default initial type of internal data in BinaryDataEngine class the same as system type.
     */
    enum DATA_ENDIAN_TYPE : uint8_t
    {
        DATA_BIG_ENDIAN = 0x01,     // First byte of binary representation of the multibyte data-type is stored first.
        DATA_LITTLE_ENDIAN = 0x02,  // Last byte of binary representation of the multibyte data-type is stored first.
        DATA_SYSTEM_ENDIAN = 0xFF   // This endian type determine the system endian type and using only in constructors.
    };

    /**
     * @enum DATA_HANDLING_MODE
     * @brief Type of the data handling mode in BinaryDataEngine class.
     *
     * @note Default initial type in BinaryDataEngine class is DATA_MODE_DEPENDENT and DATA_MODE_SAFE_OPERATOR.
     * @note Data handling mode DATA_MODE_INDEPENDENT needs specially for analyze unstructured data.
     * @note Changed only the method of processing data. Data representation does not changes after change the handling mode.
     */
    enum DATA_HANDLING_MODE : uint8_t
    {
        DATA_MODE_DEPENDENT = 0x01,        // Any data modification depends on the ending type.
        DATA_MODE_INDEPENDENT = 0x02,      // Any data modification does not depend on the ending type.
        DATA_MODE_SAFE_OPERATOR = 0x04,    // Any data modification by assign logical operators does not lead to a change the data length.
        DATA_MODE_UNSAFE_OPERATOR = 0x08,  // Any data modification by assign logical operators leads to a change the data length.
        DATA_MODE_ALLOCATION = 0x10,       // In this mode a new object of BinaryDataEngine class is created and memory allocated. The memory is cleared in the destructor.
        DATA_MODE_NO_ALLOCATION = 0x20,    // In this mode there is no creation of a new BinaryDataEngine object. The memory is not cleared in the destructor.
        DATA_MODE_DEFAULT = DATA_MODE_DEPENDENT | DATA_MODE_SAFE_OPERATOR | DATA_MODE_ALLOCATION  // Default data mode which is used in constructors.
    };


    /**
      * @fn static inline DATA_ENDIAN_TYPE CheckSystemEndian() noexcept;
      * @brief Method that checks the endian type on the system.
      * @return DATA_LITTLE_ENDIAN(0x02) - if on the system little endian, otherwise - DATA_BIG_ENDIAN(0x01).
      */
    static inline DATA_ENDIAN_TYPE CheckSystemEndian(void) noexcept
    {
        union {
            const uint16_t value;
            const uint8_t data[sizeof(uint16_t)];
        } endian { 0x0102 };
        return static_cast<DATA_ENDIAN_TYPE>(endian.data[0]);
    }


    /**
     * @class BinaryDataEngine   BinaryDataEngine.hpp   "include/analyzer/BinaryDataEngine.hpp"
     * @brief Main class of analyzer framework that contains binary data and gives an interface to work with it.
     *
     * @note This class is cross-platform.
     */
    class BinaryDataEngine
    {
        //friend class BinaryDataEngineIterator<8>;
        //friend class BinaryDataEngineConstIterator<BinaryDataEngine>;

    public:
        /**
         * @var static constexpr std::size_t npos;
         * @brief Variable that indicates about the end of sequence.
         */
        static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();

        /**
         * @var static const DATA_ENDIAN_TYPE system_endian;
         * @brief Variable that stores the system endian.
         */
        static const DATA_ENDIAN_TYPE system_endian;


        using value_type = std::byte;

        //using iterator = BinaryDataEngineIterator<BinaryDataEngine>;
        //using reverse_iterator = std::reverse_iterator<iterator>;

        //using const_iterator = BinaryDataEngineConstIterator<BinaryDataEngine>;
        //using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    private:
        /**
         * @class BitStreamEngine   BinaryDataEngine.hpp   "include/analyzer/BinaryDataEngine.hpp"
         * @brief Class that operates on a sequence of bits and offers an interface for working with them.
         *
         * @attention This class MUST BE initialized in all constructors of owner class.
         */
        class BitStreamEngine
        {
            friend class BinaryDataEngine;

        private:
            /**
             * @var const BinaryDataEngine & storedData;
             * @brief Const lvalue reference of the BinaryDataEngine owner class.
             */
            const BinaryDataEngine & storedData;

            /**
             * @fn std::pair<std::size_t, std::byte> BitStreamEngine::GetBitPosition (std::size_t) const noexcept;
             * @brief Method that returns the correct position of selected bit in stored raw data in any data endian.
             * @param [in] index - Index of bit in binary sequence of stored data.
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
             * @fn explicit BitStreamEngine::BitStreamEngine (const BinaryDataEngine &) noexcept;
             * @brief Constructor of nested BitStreamEngine class.
             * @param [in] owner - Const lvalue reference of BinaryDataEngine owner class.
             */
            explicit BitStreamEngine (const BinaryDataEngine& owner) noexcept
                    : storedData(owner)
            { }

            /**
             * @fn BitStreamEngine::~BitStreamEngine() noexcept;
             * @brief Default destructor.
             */
            ~BitStreamEngine(void) noexcept = default;

            /**
             * @fn inline std::size_t BitStreamEngine::Length() const noexcept;
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
             * @note Method returns BinaryDataEngine::npos value if an error occurred.
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
             * @note Method returns BinaryDataEngine::npos value if there are no set bits on the specified interval.
             * @note Method returns BinaryDataEngine::npos value if an error occurred.
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
             * @note Method returns BinaryDataEngine::npos value if there are no set bits on the specified interval.
             * @note Method returns BinaryDataEngine::npos value if an error occurred.
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
             * @fn template <uint8_t Mode, DATA_ENDIAN_TYPE Endian, typename Type>
             * bool BitStreamEngine::SetBitSequence (const Type, std::size_t, std::size_t, const std::size_t) const noexcept;
             * @brief Method that sets the sequence of bit under the specified indexes.
             * @tparam [in] Mode - Type of input data handling mode. Default: DATA_MODE_DEPENDENT.
             * @tparam [in] Endian - Endian of input data. Default: Local System Type (DATA_SYSTEM_ENDIAN).
             * @tparam [in] Type - Typename of copied data. The value must be an arithmetic type.
             * @tparam [in] value - Arithmetic C++ standard value for copy.
             * @param [in] position - Index in stored data from which new data will be assigned. Default: 0.
             * @param [in] first - First index of bit in input value from which sequent bits will be copied. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be copied. Default: Last bit in value.
             * @return True - if data assignment is successful, otherwise - false.
             */
            template <uint8_t Mode = DATA_MODE_DEPENDENT, DATA_ENDIAN_TYPE Endian = DATA_SYSTEM_ENDIAN, typename Type>
            bool SetBitSequence (const Type value, std::size_t position = 0, std::size_t first = 0, const std::size_t last = sizeof(Type) * 8 - 1) const noexcept
            {
                static_assert(std::is_arithmetic<Type>::value == true, "It is not possible to use not arithmetic type for this method.");

                const std::size_t size = last - first + 1;
                if (sizeof(Type) * 8 < size || position + size > Length()) {
                    return false;
                }

                BinaryDataEngine sequence(Mode, (Endian == DATA_SYSTEM_ENDIAN) ? system_endian : Endian);
                sequence.AssignData<Type>(&value, 1);

                while (first <= last) {
                    Set(position++, sequence.BitsTransform().Test(first++));
                }
                return true;
            }

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
             * @fn const BitStreamEngine & BitStreamEngine::XorBlock (const BinaryDataEngine &, std::size_t, std::size_t) const noexcept;
             * @brief Method that applies the operation XOR to the range of bits under the specified first/last indexes.
             * @param [in] other - Const lvalue reference of the BinaryDataEngine class.
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be xored. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be xored. Default: npos.
             * @return Const lvalue reference of BitStreamEngine class.
             */
            const BitStreamEngine & XorBlock (const BinaryDataEngine & /*other*/, std::size_t /*first*/ = 0, std::size_t /*last*/ = npos) const noexcept;

            /**
             * @fn std::string BitStreamEngine::ToString (std::size_t, std::size_t) const noexcept;
             * @brief Method that outputs internal binary data in string format.
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be outputed. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be outputed. Default: npos.
             * @return STL string object with sequence of the bit character representation of stored data.
             *
             * @note Data is always outputs in DATA_BIG_ENDIAN endian type if data handling mode type is DATA_MODE_DEPENDENT.
             */
            std::string ToString (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos) const noexcept;

            /**
             * @fn inline bool BitStreamEngine::operator[] (const std::size_t) const noexcept;
             * @brief Operator that returns the value of bit under the specified index.
             * @param [in] index - Index of bit in binary sequence of stored data.
             * @return Value of the selected bit.
             */
            inline bool operator[] (const std::size_t index) const noexcept { return Test(index); }

            /**
             * @fn explicit operator BitStreamEngine::bool() const noexcept;
             * @brief Operator that returns bit sequence characteristic when all of the bits are set.
             * @return True - if all of the bits of stored data are set, otherwise - false.
             */
            explicit operator bool(void) const noexcept { return All(); }

            /**
             * @fn friend inline std::ostream & operator<< (std::ostream &, const BitStreamEngine &) noexcept;
             * @brief Operator that outputs internal binary raw data in binary string format.
             * @param [in,out] stream - Reference of the output stream engine.
             * @param [in] engine - Const lvalue reference of the BitStreamEngine class.
             * @return Lvalue reference of the inputted STL std::ostream class.
             *
             * @note Data is always outputs in DATA_BIG_ENDIAN endian type if data handling mode type is DATA_MODE_DEPENDENT.
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
                                if (idx % 8 == 0) { stream << ' '; }
                            }
                            stream << engine.Test(0);
                        }
                        else  // If data handling mode type is DATA_MODE_INDEPENDENT.
                        {
                            for (std::size_t idx = 0; idx < engine.Length(); ++idx) {
                                if (idx % 8 == 0 && idx != 0) { stream << ' '; }
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
             * @note If data handling mode type is DATA_MODE_SAFE_OPERATOR then the size of result data will be preserved.
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
             * @note If data handling mode type is DATA_MODE_SAFE_OPERATOR then the size of result data will be preserved.
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
             * @note If data handling mode type is DATA_MODE_SAFE_OPERATOR then the size of result data will be preserved.
             *
             * @attention If operands have different endian depended handling mode then no changes will be made.
             */
            const BitStreamEngine & operator^= (const BitStreamEngine & /*other*/) const noexcept;

            /**
             * @fn friend inline BinaryDataEngine BitStreamEngine::operator& (const BitStreamEngine &, const BitStreamEngine &) noexcept;
             * @brief Logical bitwise AND operator that transforms internal binary raw data.
             * @param [in] left - Const lvalue reference of the BitStreamEngine class as left operand.
             * @param [in] right - Const lvalue reference of the BitStreamEngine class as right operand.
             * @return New temporary object of transformed (by operation AND) BinaryDataEngine class.
             *
             * @note If operands have different endian and mode then result BinaryDataEngine will have endian and mode of the left operand.
             * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LESSER length.
             *
             * @attention Result BinaryDataEngine class always processing in DATA_MODE_UNSAFE_OPERATOR mode.
             */
            friend inline BinaryDataEngine operator& (const BitStreamEngine& left, const BitStreamEngine& right) noexcept
            {
                BinaryDataEngine result(left.storedData);
                result.SetDataModeType(types::DATA_MODE_UNSAFE_OPERATOR);
                if (result.IsEmpty() == false) {
                    result.BitsTransform() &= right;
                }
                result.SetDataModeType(left.storedData.DataModeType());
                return result;
            }

            /**
             * @fn friend inline BinaryDataEngine BitStreamEngine::operator| (const BitStreamEngine &, const BitStreamEngine &) noexcept;
             * @brief Logical bitwise OR operator that transforms internal binary raw data.
             * @param [in] left - Const lvalue reference of the BitStreamEngine class as left operand.
             * @param [in] right - Const lvalue reference of the BitStreamEngine class as right operand.
             * @return New temporary object of transformed (by operation OR) BinaryDataEngine class.
             *
             * @note If operands have different endian and mode then result BinaryDataEngine will have endian and mode of the left operand.
             * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LONGER length.
             *
             * @attention Result BinaryDataEngine class always processing in DATA_MODE_UNSAFE_OPERATOR mode.
             */
            friend inline BinaryDataEngine operator| (const BitStreamEngine& left, const BitStreamEngine& right) noexcept
            {
                BinaryDataEngine result(left.storedData);
                result.SetDataModeType(types::DATA_MODE_UNSAFE_OPERATOR);
                if (result.IsEmpty() == false) {
                    result.BitsTransform() |= right;
                }
                result.SetDataModeType(left.storedData.DataModeType());
                return result;
            }

            /**
             * @fn friend inline BinaryDataEngine BitStreamEngine::operator^ (const BitStreamEngine &, const BitStreamEngine &) noexcept;
             * @brief Logical bitwise XOR operator that transforms internal binary raw data.
             * @param [in] left - Const lvalue reference of the BitStreamEngine class as left operand.
             * @param [in] right - Const lvalue reference of the BitStreamEngine class as right operand.
             * @return New temporary object of transformed (by operation XOR) BinaryDataEngine class.
             *
             * @note If operands have different endian and mode then result BinaryDataEngine will have endian and mode of the left operand.
             * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LONGER length.
             *
             * @attention Result BinaryDataEngine class always processing in DATA_MODE_UNSAFE_OPERATOR mode.
             */
            friend inline BinaryDataEngine operator^ (const BitStreamEngine& left, const BitStreamEngine& right) noexcept
            {
                BinaryDataEngine result(left.storedData);
                result.SetDataModeType(types::DATA_MODE_UNSAFE_OPERATOR);
                if (result.IsEmpty() == false) {
                    result.BitsTransform() ^= right;
                }
                result.SetDataModeType(left.storedData.DataModeType());
                return result;
            }

            /**
             * @fn friend inline BinaryDataEngine BitStreamEngine::operator<< (const BitStreamEngine &, const std::size_t) noexcept;
             * @brief Bitwise left shift operator that performs direct left bit shift by a specified bit offset.
             * @param [in] engine - Const lvalue reference of the BitStreamEngine class as left operand.
             * @param [in] shift - Bit offset for direct left bit shift as right operand.
             * @return New temporary object of transformed BinaryDataEngine class.
             */
            friend inline BinaryDataEngine operator<< (const BitStreamEngine& engine, const std::size_t shift) noexcept
            {
                BinaryDataEngine result(engine.storedData);
                result.BitsTransform().ShiftLeft(shift, false);
                return result;
            }

            /**
             * @fn friend inline BinaryDataEngine BitStreamEngine::operator>> (const BitStreamEngine &, const std::size_t) noexcept;
             * @brief Bitwise right shift operator that performs direct right bit shift by a specified bit offset.
             * @param [in] engine - Const lvalue reference of the BitStreamEngine class as left operand.
             * @param [in] shift - Bit offset for direct right bit shift as right operand.
             * @return New temporary object of transformed BinaryDataEngine class.
             */
            friend inline BinaryDataEngine operator>> (const BitStreamEngine& engine, const std::size_t shift) noexcept
            {
                BinaryDataEngine result(engine.storedData);
                result.BitsTransform().ShiftRight(shift, false);
                return result;
            }

            /**
             * @fn friend inline BinaryDataEngine BitStreamEngine::operator~ (const BitStreamEngine &) const noexcept;
             * @brief Logical bitwise complement operator that inverts each bit in internal binary raw data.
             * @param [in] engine - Const lvalue reference of the BitStreamEngine class.
             * @return New temporary object of transformed (by operation NOT) BinaryDataEngine class.
             */
            friend inline BinaryDataEngine operator~ (const BitStreamEngine& engine) noexcept
            {
                BinaryDataEngine result(engine.storedData);
                result.BitsTransform().InvertBlock();
                return result;
            }

        };


        /**
         * @class ByteStreamEngine   BinaryDataEngine.hpp   "include/analyzer/BinaryDataEngine.hpp"
         * @brief Class that operates on a sequence of bytes and offers an interface for working with them.
         *
         * @attention This class MUST BE initialized in all constructors of owner class.
         */
        class ByteStreamEngine
        {
            friend class BinaryDataEngine;

        private:
            /**
             * @var const BinaryDataEngine & storedData;
             * @brief Const lvalue reference of the BinaryDataEngine owner class.
             */
            const BinaryDataEngine & storedData;

            /**
             * @fn std::size_t ByteStreamEngine::GetBytePosition (std::size_t) const noexcept;
             * @brief Method that returns the correct position of selected byte in stored raw data in any data endian.
             * @param [in] index - Index of byte in byte sequence of stored data.
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
             * @fn explicit ByteStreamEngine::ByteStreamEngine (const BinaryDataEngine &) noexcept;
             * @brief Constructor of nested ByteStreamEngine class.
             * @param [in] owner - Const lvalue reference of BinaryDataEngine owner class.
             */
            explicit ByteStreamEngine (const BinaryDataEngine& owner) noexcept
                    : storedData(owner)
            { }

            /**
             * @fn ByteStreamEngine::~ByteStreamEngine() noexcept;
             * @brief Default destructor.
             */
            ~ByteStreamEngine(void) noexcept = default;

            /**
             * @fn inline std::size_t ByteStreamEngine::Length() const noexcept;
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
             * @param [in] index - Index of byte in byte sequence of stored data.
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
         * @var uint8_t dataModeType;
         * @brief Handling mode type of stored data.
         */
        uint8_t dataModeType = DATA_MODE_DEFAULT;
        /**
         * @var DATA_ENDIAN_TYPE dataEndianType;
         * @brief Endian type of stored data.
         */
        DATA_ENDIAN_TYPE dataEndianType = system_endian;
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


    public:
        /**
         * @fn explicit BinaryDataEngine::BinaryDataEngine (uint8_t, DATA_ENDIAN_TYPE noexcept;
         * @brief Constructor of BinaryDataEngine class.
         * @param [in] mode - Type of the data handling mode. Default: DATA_DEFAULT_MODE.
         * @param [in] endian - Endian of stored data. Default: Local System Type.
         */
        explicit BinaryDataEngine (uint8_t mode = DATA_MODE_DEFAULT, DATA_ENDIAN_TYPE endian = system_endian) noexcept
                : dataModeType(mode), dataEndianType(endian), bitStreamTransform(*this), byteStreamTransform(*this)
        { }

        /**
         * @fn BinaryDataEngine::~BinaryDataEngine() noexcept;
         * @brief Destructor of BinaryDataEngine class.
         */
        ~BinaryDataEngine(void) noexcept;

        /**
         * @fn BinaryDataEngine::BinaryDataEngine (const BinaryDataEngine &) noexcept;
         * @brief Copy assignment constructor of BinaryDataEngine class.
         * @param [in] other - Const lvalue reference of copied BinaryDataEngine class.
         *
         * @attention Need to check the size of data after use this constructor because it is 'noexcept'.
         */
        BinaryDataEngine (const BinaryDataEngine & /*other*/) noexcept;

        /**
         * @fn BinaryDataEngine::BinaryDataEngine (BinaryDataEngine &&) noexcept;
         * @brief Move assignment constructor of BinaryDataEngine class.
         * @param [in] other - Rvalue reference of moved BinaryDataEngine class.
         */
        BinaryDataEngine (BinaryDataEngine && /*other*/) noexcept;

        /**
         * @fn explicit BinaryDataEngine::BinaryDataEngine (std::size_t, uint8_t, DATA_ENDIAN_TYPE) noexcept;
         * @brief Constructor that allocates specified amount of bytes.
         * @param [in] size - Number of bytes for allocate.
         * @param [in] mode - Type of the data handling mode. Default: DATA_DEFAULT_MODE.
         * @param [in] endian - Endian of stored data. Default: Local System Type.
         *
         * @attention Need to check the size of data after use this method because it is 'noexcept'.
         */
        explicit BinaryDataEngine (std::size_t /*size*/, uint8_t /*mode*/ = DATA_MODE_DEFAULT, DATA_ENDIAN_TYPE /*endian*/ = system_endian) noexcept;

        /**
         * @fn explicit BinaryDataEngine::BinaryDataEngine (std::byte * const, std::size_t, DATA_ENDIAN_TYPE, uint8_t) noexcept;
         * @brief Constructor that accepts a pointer to allocated binary data.
         * @param [in] memory - Pointer to allocated data.
         * @param [in] size - Number of bytes in data.
         * @param [in] endian - Endian of inputted data. Default: Local System Type.
         * @param [in] mode - Type of the data handling mode. Default: DATA_DEFAULT_MODE.
         */
        explicit BinaryDataEngine (std::byte * const /*memory*/,
                                   std::size_t /*size*/,
                                   DATA_ENDIAN_TYPE /*endian*/ = system_endian,
                                   uint8_t /*mode*/ = DATA_MODE_DEFAULT) noexcept;

        /**
         * @fn BinaryDataEngine & BinaryDataEngine::operator= (const BinaryDataEngine &) noexcept;
         * @brief Copy assignment operator of BinaryDataEngine class.
         * @param [in] other - Const lvalue reference of copied BinaryDataEngine class.
         * @return Lvalue reference of copied BinaryDataEngine class.
         *
         * @attention Need to check the size of data after use this operator because it is 'noexcept'.
         */
        BinaryDataEngine & operator= (const BinaryDataEngine & /*other*/) noexcept;

        /**
         * @fn BinaryDataEngine & BinaryDataEngine::operator= (BinaryDataEngine &&) noexcept;
         * @brief Move assignment operator of BinaryDataEngine class.
         * @param [in] other - Rvalue reference of moved BinaryDataEngine class.
         * @return Lvalue reference of moved BinaryDataEngine class.
         */
        BinaryDataEngine & operator= (BinaryDataEngine && /*other*/) noexcept;

        /**
         * @fn template <typename Type>
         * bool BinaryDataEngine::AssignData (const Type *, const std::size_t) noexcept;
         * @brief Method that assigns data to BinaryDataEngine.
         * @tparam [in] Type - Typename of assigned data.
         * @param [in] memory - Pointer to the constant data of selected type.
         * @param [in] count - The number of elements in the input data of selected type.
         * @return True - if data assignment is successful, otherwise - false.
         *
         * @note Input type MUST be a POD type.
         * @note Return value is marked with the "nodiscard" attribute.
         */
        template <typename Type>
        [[nodiscard]]
        bool AssignData (const Type* memory, const std::size_t count) noexcept
        {
            static_assert(is_pod_type<Type>::value == true, "It is not possible to use not POD type for this method.");

            if (memory == nullptr) { return false; }

            if (length != sizeof(Type))
            {
                length = count * sizeof(Type);
                data = system::allocMemoryForArray<std::byte>(length, memory, length);
                if (data == nullptr) {
                    length = 0;
                    return false;
                }
            }
            else { memcpy(data.get(), memory, length); }
            return true;
        }

        /**
         * @fn template <typename Type>
         * bool BinaryDataEngine::AssignData (const Type, const Type) noexcept;
         * @brief Method that assigns data to BinaryDataEngine.
         * @tparam [in] Type - Typename of assigned data.
         * @param [in] begin - Iterator to the first element of const data of selected type.
         * @param [in] end - Iterator to the element following the last one of const data of selected type.
         * @return True - if data assignment is successful, otherwise - false.
         *
         * @note Input type under iterator MUST be a POD type.
         * @note Return value is marked with the "nodiscard" attribute.
         */
        template <typename Type>
        [[nodiscard]]
        bool AssignData (const Type begin, const Type end) noexcept
        {
            static_assert(is_iterator_type<Type>::value == true &&
                                  is_pod_type<typename std::iterator_traits<Type>::value_type>::value == true,
                          "It is not possible to use not Iterator type for this method.");

            length = static_cast<std::size_t>(std::distance(begin, end)) * sizeof(typename std::iterator_traits<Type>::value_type);
            data = system::allocMemoryForArray<std::byte>(length, &(*begin), length);
            if (data == nullptr) {
                length = 0;
                return false;
            }
            return true;
        }

        /**
         * @fn const BinaryDataEngine::BitStreamEngine & BinaryDataEngine::BitsTransform() const noexcept;
         * @brief Method that returns const reference of the nested BitStreamEngine class for working with bits.
         * @return Const lvalue reference of the BitStreamEngine class.
         */
        const BitStreamEngine& BitsTransform(void) const noexcept { return bitStreamTransform; }

        /**
         * @fn const BinaryDataEngine::ByteStreamEngine & BinaryDataEngine::BytesTransform() const noexcept;
         * @brief Method that returns const reference of the nested ByteStreamEngine class for working with bytes.
         * @return Const lvalue reference of the ByteStreamEngine class.
         */
        const ByteStreamEngine& BytesTransform(void) const noexcept { return byteStreamTransform; }

        /**
         * @fn inline std::size_t BinaryDataEngine::Size() const noexcept
         * @brief Method that returns the size of data.
         * @return Size of stored data in bytes.
         */
        inline std::size_t Size(void) const noexcept { return length; }

        /**
         * @fn inline const std::byte * BinaryDataEngine::Data() const noexcept;
         * @brief Method that returns the pointer to the const internal data.
         * @return Pointer to the const internal data.
         */
        inline const std::byte* Data(void) const noexcept { return data.get(); }

        /**
         * @fn inline bool BinaryDataEngine::IsEmpty() const noexcept;
         * @brief Method that returns the state of stored data in BinaryDataEngine class.
         * @return True - if stored data is empty, otherwise - false.
         */
        inline bool IsEmpty(void) const noexcept { return length == 0; }

        /**
         * @fn inline uint8_t BinaryDataEngine::DataModeType() const noexcept;
         * @brief Method that returns the data handling mode type of stored data in BinaryDataEngine class.
         * @return The set of enabled modes of DATA_HANDLING_MODE enum.
         */
        inline uint8_t DataModeType(void) const noexcept { return dataModeType; }

        /**
         * @fn inline DATA_ENDIAN_TYPE BinaryDataEngine::DataEndianType() const noexcept;
         * @brief Method that returns the endian type of stored data in BinaryDataEngine class.
         * @return DATA_LITTLE_ENDIAN(0x02) - if on the system little endian, otherwise - DATA_BIG_ENDIAN(0x01).
         */
        inline DATA_ENDIAN_TYPE DataEndianType(void) const noexcept { return dataEndianType; }

        /**
         * @fn void BinaryDataEngine::SetDataModeType (uint8_t) noexcept;
         * @brief Method that changes handling mode type of stored data in BinaryDataEngine class.
         * @param [in] mode - New data handling mode type.
         *
         * @note Changed only the method of processing data. Data representation does not changes.
         * @note After setting a new mode then the opposite mode will be automatically turned off.
         */
        void SetDataModeType (uint8_t /*mode*/) noexcept;

        /**
         * @fn void BinaryDataEngine::SetDataEndianType (DATA_ENDIAN_TYPE, bool) noexcept;
         * @brief Method that changes endian type of stored data in BinaryDataEngine class.
         * @param [in] endian - New data endian type.
         * @param [in] convert - Flag indicating whether to change the presentation of the stored data or not. Default: true.
         */
        void SetDataEndianType (DATA_ENDIAN_TYPE /*endian*/, bool /*convert*/ = true) noexcept;

        /**
         * @fn explicit operator BinaryDataEngine::bool() const noexcept;
         * @brief Operator that returns the internal state of BinaryDataEngine class.
         * @return True - if BinaryDataEngine class is not empty, otherwise - false.
         */
        explicit operator bool(void) const noexcept { return length != 0; }

        /**
         * @fn inline const std::byte & BinaryDataEngine::operator[] (std::size_t) const noexcept;
         * @brief Operator that returns a const reference to an element by selected index.
         * @param [in] index - Index of byte in byte sequence of stored data.
         * @return Return a const reference to an element by selected index.
         *
         * @note This method does not consider the type of endian in which data are presented.
         *
         * @attention Undefined behavior error may occur when using this method (Out-Of-Range).
         */
        inline const std::byte& operator[] (const std::size_t index) const noexcept { return data[index]; }

        /**
         * @fn std::byte * BinaryDataEngine::GetAt (std::size_t) const noexcept;
         * @brief Method that returns a pointer to an element by selected index.
         * @param [in] index - Index of byte in byte sequence of stored data.
         * @return Return a pointer to an element by selected index or nullptr in an error occurred.
         *
         * @note This method does not consider the type of endian in which data are presented.
         */
        std::byte * GetAt (std::size_t /*index*/) const noexcept;

        /**
         * @fn void BinaryDataEngine::Clear() noexcept;
         * @brief Method that clears the data.
         */
        void Clear(void) noexcept;

    };


}  // namespace types.


#endif  // PROTOCOL_ANALYZER_RAW_DATA_BUFFER_HPP
