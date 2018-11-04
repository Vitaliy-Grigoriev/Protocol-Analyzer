// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#ifndef PROTOCOL_ANALYZER_BINARY_DATA_ENGINE_HPP
#define PROTOCOL_ANALYZER_BINARY_DATA_ENGINE_HPP

#include <map>  // std::pair.
#include <ostream>  // std::ostream.

#include "System.hpp"  // system::allocMemoryForArray.
#include "Common.hpp"  // common::is_pod_type, common::is_iterator_type, common::is_supports_binary_operations, std::is_default_constructible.

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
//  Reverse Big Endian Model.
//  |24______31|16______23|8______15|0______7|
//
//  Endian Independent Model.
//  |0______7|8______15|16______23|24______31|
//
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


namespace analyzer::framework::common::types
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
        // Not implemented soon.
        DATA_MODE_OPERATOR_ALIGN_LOW_ORDER = 0x40,  // Any data modification by assign logical operators goes in order from low to high.
        DATA_MODE_OPERATOR_ALIGN_HIGH_ORDER = 0x80,  // Any data modification by assign logical operators goes in order from high to low.
        DATA_MODE_DEFAULT = DATA_MODE_DEPENDENT | DATA_MODE_SAFE_OPERATOR | DATA_MODE_ALLOCATION | DATA_MODE_OPERATOR_ALIGN_LOW_ORDER  // Default data mode which is used in constructors.
    };


    /**
     * @brief Method that checks the endian type on the system.
     *
     * @return DATA_LITTLE_ENDIAN(0x02) - if on the system little endian, otherwise - DATA_BIG_ENDIAN(0x01).
     */
    static inline DATA_ENDIAN_TYPE CheckSystemEndian(void) noexcept
    {
        const uint16_t value = 0x0102;
        return static_cast<DATA_ENDIAN_TYPE>(static_cast<const uint8_t&>(value));
    }


    /**
     * @class BinaryStructuredDataEngine   BinaryStructuredDataEngine.hpp   "include/framework/BinaryStructuredDataEngine.hpp"
     * @brief Forward declaration of BinaryStructuredDataEngine class.
     */
    class BinaryStructuredDataEngine;


    /**
     * @class BinaryDataEngine   BinaryDataEngine.hpp   "include/framework/BinaryDataEngine.hpp"
     * @brief Main class of analyzer framework that contains binary data and gives an interface to work with it.
     *
     * @note This class is cross-platform.
     */
    class BinaryDataEngine
    {
        friend class BinaryStructuredDataEngine;
        //friend class BinaryDataEngineIterator<8>;
        //friend class BinaryDataEngineConstIterator<BinaryDataEngine>;

    public:
        /**
         * @brief Variable that indicates about the end of sequence.
         */
        static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();
        /**
         * @brief Variable that stores system endian.
         */
        static const DATA_ENDIAN_TYPE system_endian;


        using value_type = std::byte;

        //using iterator = BinaryDataEngineIterator<BinaryDataEngine>;
        //using reverse_iterator = std::reverse_iterator<iterator>;

        //using const_iterator = BinaryDataEngineConstIterator<BinaryDataEngine>;
        //using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    private:
        /**
         * @class BitStreamInformationEngine   BinaryDataEngine.hpp   "include/framework/BinaryDataEngine.hpp"
         * @brief Class that operates on a sequence of bits and offers an interface for working with them.
         *
         * @attention This class MUST BE initialized in all constructors of owner class.
         */
        class BitStreamInformationEngine
        {
            friend class BinaryDataEngine;
            friend class BitStreamTransformEngine;
            friend class BinaryStructuredDataEngine;

        private:
            /**
             * @brief Const pointer to the const BinaryDataEngine owner class.
             */
            const BinaryDataEngine * const storedData;

            /**
             * @brief Method that returns the correct position of selected bit in stored binary data in any data endian.
             *
             * @param [in] index - Index of bit in stored binary data.
             * @return Index of element in array of binary stored data and shift to this bit in selected part.
             *
             * @attention Before using this method, MUST be checked that the index does not out-of-range.
             */
            std::pair<std::size_t, std::byte> GetBitPosition (std::size_t /*index*/) const noexcept;

            /**
             * @brief Method that returns bit value under the specified index.
             *
             * @param [in] index - Index of bit in stored binary data.
             * @return Value of the selected bit.
             *
             * @attention Before using this method, MUST be checked that the index does not out-of-range.
             */
            bool GetBitValue (std::size_t /*index*/) const noexcept;

        public:
            BitStreamInformationEngine(void) = delete;
            BitStreamInformationEngine (BitStreamInformationEngine &&) = delete;
            BitStreamInformationEngine (const BitStreamInformationEngine &) = delete;
            BitStreamInformationEngine & operator= (BitStreamInformationEngine &&) = delete;
            BitStreamInformationEngine & operator= (const BitStreamInformationEngine &) = delete;

            /**
             * @brief Constructor of nested BitStreamInformationEngine class.
             *
             * @param [in] owner - Const poiner to the const BinaryDataEngine owner class.
             */
            explicit BitStreamInformationEngine (const BinaryDataEngine* const owner) noexcept
                : storedData(owner)
            { }

            /**
             * @brief Default destructor of nested BitStreamInformationEngine class.
             */
            ~BitStreamInformationEngine(void) noexcept = default;

            /**
             * @brief Method that returns the length of stored data in bits.
             *
             * @return Length of bit sequence of stored data.
             */
            inline std::size_t Length(void) const noexcept { return storedData->length * 8; }

            /**
             * @brief Method that checks the bit under the specified index.
             *
             * @param [in] index - Index of bit in binary sequence.
             * @return Boolean value that indicates about the value of the selected bit.
             *
             * @warning Method always returns 'false' if the index is out-of-range.
             */
            bool Test (std::size_t /*index*/) const noexcept;

            /**
             * @brief Method that returns bit sequence characteristic when all bits are set in block of stored data.
             *
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be checked. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which (inclusive)bits will be checked. Default: npos.
             * @return TRUE - if all bits in block of stored data are set, otherwise - FALSE.
             *
             * @warning Method always returns 'false' if the index is out-of-range.
             */
            bool All (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos) const noexcept;

            /**
             * @brief Method that returns bit sequence characteristic when any of the bits are set in block of stored data.
             *
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be checked. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which (inclusive) bits will be checked. Default: npos.
             * @return TRUE - if any of the bits in block of stored data are set, otherwise - FALSE.
             *
             * @warning Method always returns 'false' if the index is out-of-range.
             */
            bool Any (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos) const noexcept;

            /**
             * @brief Method that returns bit sequence characteristic when none of the bits are set in block of stored data.
             *
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be checked. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which (inclusive) bits will be checked. Default: npos.
             * @return TRUE - if none of the bits in block of stored data are set, otherwise - FALSE.
             *
             * @warning Method always returns 'false' if the index is out-of-range.
             */
            bool None (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos) const noexcept;

            /**
             * @brief Method that returns the number of bits that are set in the selected interval of stored data.
             *
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be checked. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be checked. Default: npos.
             * @return Number of bits that are set in block of stored data.
             *
             * @note Method returns 'npos' value if an error occurred.
             */
            std::size_t Count (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos) const noexcept;

            /**
             * @brief Method that returns position of the first set bit in the selected interval of stored data.
             *
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be checked. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be checked. Default: npos.
             * @param [in] isRelative - Boolean flag that indicates about the type of return index. Default: true.
             * @return Position of the first set bit in the selected interval of stored data.
             *
             * @note Method returns 'npos' value if there are no set bits on the specified interval.
             */
            std::optional<std::size_t> GetFirstIndex (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos, bool isRelative = true) const noexcept;

            /**
             * @brief Method that returns position of the last set bit in the selected interval of stored data.
             *
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be checked. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be checked. Default: npos.
             * @param [in] isRelative - Boolean flag that indicates about the type of return index. Default: true.
             * @return Position of the last set bit in the selected interval of stored data.
             *
             * @note Method returns 'npos' value if there are no set bits on the specified interval.
             */
            std::optional<std::size_t> GetLastIndex (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos, bool isRelative = true) const noexcept;

            /**
             * @brief Method that outputs internal binary data in string format.
             *
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be outputted. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be outputted. Default: npos.
             * @return STL string object with sequence of the bit character representation of stored binary data.
             *
             * @note Data is always outputs in DATA_BIG_ENDIAN endian type if data handling mode type is DATA_MODE_DEPENDENT.
             */
            std::string ToString (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos) const noexcept;

            /**
             * @brief Operator that returns the value of bit under the specified index.
             *
             * @param [in] index - Index of bit in binary sequence of stored data.
             * @return Value of the selected bit in stored binary data.
             *
             * @warning Method always returns 'false' if the index is out-of-range.
             */
            inline bool operator[] (const std::size_t index) const noexcept { return Test(index); }

            /**
             * @brief Operator that returns bit sequence characteristic when all of the bits are set.
             *
             * @return TRUE - if all of the bits of stored data are set, otherwise - FALSE.
             */
            explicit operator bool(void) const noexcept { return All(); }

            /**
             * @brief Operator that outputs internal binary data in binary string format.
             *
             * @param [in,out] stream - Reference of the output stream engine.
             * @param [in] engine - Const lvalue reference of the BitStreamTransformEngine class.
             * @return Lvalue reference of the inputted STL std::ostream class.
             *
             * @note Data is always outputs in DATA_BIG_ENDIAN endian type if data handling mode type is DATA_MODE_DEPENDENT.
             */
            friend inline std::ostream& operator<< (std::ostream& stream, const BitStreamInformationEngine& engine) noexcept
            {
                try
                {
                    if (engine.storedData->IsEmpty() == false)
                    {
                        stream.unsetf(std::ios_base::boolalpha);
                        if ((engine.storedData->DataModeType() & DATA_MODE_DEPENDENT) != 0U)
                        {
                            for (std::size_t idx = engine.Length() - 1; idx != 0; --idx) {
                                stream << engine.GetBitValue(idx);
                                if (idx % 8 == 0) { stream << ' '; }
                            }
                            stream << engine.GetBitValue(0);
                        }
                        else  // If data handling mode type is DATA_MODE_INDEPENDENT.
                        {
                            for (std::size_t idx = 0; idx < engine.Length(); ++idx) {
                                if (idx % 8 == 0 && idx != 0) { stream << ' '; }
                                stream << engine.GetBitValue(idx);
                            }
                        }
                    }
                }
                catch (const std::ios_base::failure& /*err*/) { }
                return stream;
            }
        };

        /**
         * @class BitStreamTransformEngine   BinaryDataEngine.hpp   "include/framework/BinaryDataEngine.hpp"
         * @brief Class that operates on a sequence of bits and offers an interface for working with them.
         *
         * @attention This class MUST BE initialized in all constructors of owner class.
         */
        class BitStreamTransformEngine
        {
            friend class BinaryDataEngine;
            friend class BitStreamInformationEngine;
            friend class BinaryStructuredDataEngine;

        private:
            /**
             * @brief Const pointer to the BinaryDataEngine owner class.
             */
            BinaryDataEngine * const storedData;

        public:
            BitStreamTransformEngine(void) = delete;
            BitStreamTransformEngine (BitStreamTransformEngine &&) = delete;
            BitStreamTransformEngine (const BitStreamTransformEngine &) = delete;
            BitStreamTransformEngine & operator= (BitStreamTransformEngine &&) = delete;
            BitStreamTransformEngine & operator= (const BitStreamTransformEngine &) = delete;

            /**
             * @brief Constructor of nested BitStreamTransformEngine class.
             *
             * @param [in] owner - Const pointer to the BinaryDataEngine owner class.
             */
            explicit BitStreamTransformEngine (BinaryDataEngine * const owner) noexcept
                : storedData(owner)
            { }

            /**
             * @brief Default destructor of nested BitStreamTransformEngine class.
             */
            ~BitStreamTransformEngine(void) noexcept = default;

            /**
             * @brief Method that returns the length of stored data in bits.
             *
             * @return Length of bit sequence of stored data.
             */
            inline std::size_t Length(void) const noexcept { return storedData->length * 8; }

            /**
             * @brief Method that performs direct left bit shift by a specified bit offset.
             *
             * @param [in] shift - Bit offset for direct left bit shift.
             * @param [in] fillBit - Value of the fill bit after the left shift. Default: false (0).
             * @return Const lvalue reference of BitStreamTransformEngine class.
             */
            BitStreamTransformEngine & ShiftLeft (std::size_t /*shift*/, bool /*fillBit*/ = false) noexcept;

            /**
             * @brief Method that performs direct right bit shift by a specified bit offset.
             *
             * @param [in] shift - Bit offset for direct right bit shift.
             * @param [in] fillBit - Value of the fill bit after the right shift. Default: false (0).
             * @return Const lvalue reference of BitStreamTransformEngine class.
             */
            BitStreamTransformEngine & ShiftRight (std::size_t /*shift*/, bool /*fillBit*/ = false) noexcept;

            /**
             * @brief Method that performs round left bit shift by a specified bit offset.
             *
             * @param [in] shift - Bit offset for round left bit shift.
             * @return Lvalue reference of BitStreamTransformEngine class.
             */
            BitStreamTransformEngine & RoundShiftLeft (std::size_t /*shift*/) noexcept;

            /**
             * @brief Method that performs round right bit shift by a specified bit offset.
             *
             * @param [in] shift - Bit offset for round right bit shift.
             * @return Lvalue reference of BitStreamTransformEngine class.
             */
            BitStreamTransformEngine & RoundShiftRight (std::size_t /*shift*/) noexcept;

            /**
             * @brief Method that sets the bit under the specified index to new value.
             *
             * @param [in] index - Index of bit in binary sequence.
             * @param [in] fillBit - New value of selected bit. Default: true (1).
             * @return Const lvalue reference of modified BitStreamTransformEngine class.
             */
            const BitStreamTransformEngine & Set (std::size_t /*index*/, bool /*fillBit*/ = true) const noexcept;

            /**
             * @brief Method that sets the sequence of bit under the specified indexes.
             *
             * @tparam [in] Mode - Type of input data dependent mode. Default: DATA_MODE_DEPENDENT.
             * @tparam [in] Endian - Endian of input data. Default: Local System Type (DATA_SYSTEM_ENDIAN).
             * @tparam [in] Type - Typename of copied data. The value must be an arithmetic type.
             * @tparam [in] value - Arithmetic C++ standard value for copy.
             * @param [in] position - Index in stored data from which new data will be assigned. Default: 0.
             * @param [in] first - First index of bit in input value from which sequent bits will be copied. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be copied. Default: Last bit in value.
             * @return TRUE - if data assignment is successful, otherwise - FALSE.
             */
            template <uint8_t Mode = DATA_MODE_DEPENDENT, DATA_ENDIAN_TYPE Endian = DATA_SYSTEM_ENDIAN, typename Type>
            bool SetBitSequence (const Type value, std::size_t position = 0, std::size_t first = 0, const std::size_t last = sizeof(Type) * 8 - 1) const noexcept
            {
                static_assert(std::is_arithmetic<Type>::value == true, "It is not possible to use not arithmetic type for this method.");
                const std::size_t size = last - first + 1;
                if (sizeof(Type) * 8 < size || position + size > Length()) {
                    return false;
                }

                BinaryDataEngine wrapper(reinterpret_cast<std::byte*>(&value), sizeof(Type), (Endian == DATA_SYSTEM_ENDIAN) ? system_endian : Endian, Mode);
                while (first <= last) {
                    Set(position++, wrapper.BitsInformation().GetBitValue(first++));
                }
                return true;
            }

            /**
             * @brief Method that reverses a sequence of bits under the specified first/last indexes.
             *
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be reversed. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be reversed. Default: npos.
             * @return Const lvalue reference of BitStreamTransformEngine class.
             */
            const BitStreamTransformEngine & Reverse (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos) const noexcept;

            /**
             * @brief Method that inverts the bit under the specified index.
             *
             * @param [in] index - Index of bit in binary sequence.
             * @return Lvalue reference of BitStreamTransformEngine class.
             */
            BitStreamTransformEngine & Invert (std::size_t /*index*/) noexcept;

            /**
             * @brief Method that inverts the range of bits under the specified first/last indexes.
             *
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be inverted. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be inverted. Default: npos.
             * @return Lvalue reference of BitStreamTransformEngine class.
             */
            BitStreamTransformEngine & InvertBlock (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos) noexcept;

            /**
             * @brief Method that converts interval of stored binary data into user type.
             *
             * @tparam [in] Type - Typename of variable to which stored data will be converted.
             * @tparam [in] Size - Number of bytes in output data (Used only if Type is a compound variable).
             * @tparam [in] Endian - Endian of output data. Default: Local System Type (DATA_SYSTEM_ENDIAN).
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be copied. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be copied. Default: npos.
             * @return Variable of selected type that consist of the bit sequence in the selected interval of stored data.
             *
             * @attention If output real data size (in case struct/class) less then sizeof(Type) then MUST specify their size.
             */
            template <typename Type, DATA_ENDIAN_TYPE Endian = DATA_SYSTEM_ENDIAN, std::size_t Size = sizeof(Type)>
            std::optional<Type> Convert (std::size_t first = 0, std::size_t last = npos) const noexcept
            {
                static_assert(std::is_default_constructible<Type>::value == true,
                              "It is not possible for this method to use type without default constructor.");

                if (last == npos) { last = Length() - 1; }
                if (first > last || last >= Length()) { return std::nullopt; }
                if (last - first + 1 > Size * 8) { return std::nullopt; }

                Type result = { };
                BinaryDataEngine wrapper(reinterpret_cast<std::byte*>(&result), Size, (Endian == DATA_SYSTEM_ENDIAN) ? system_endian : Endian);

                std::size_t position = 0;
                while (first <= last) {
                    wrapper.BitsTransform().Set(position++, storedData->BitsInformation().GetBitValue(first++));
                }
                return result;
            }

            /**
             * @brief Operator that outputs internal binary data in binary string format.
             *
             * @param [in,out] stream - Reference of the output stream engine.
             * @param [in] engine - Const lvalue reference of the BitStreamTransformEngine class.
             * @return Lvalue reference of the inputted STL std::ostream class.
             *
             * @note Data is always outputs in DATA_BIG_ENDIAN endian type if data handling mode type is DATA_MODE_DEPENDENT.
             */
            friend inline std::ostream& operator<< (std::ostream& stream, const BitStreamTransformEngine& engine) noexcept
            {
                try
                {
                    if (engine.storedData->IsEmpty() == false)
                    {
                        stream.unsetf(std::ios_base::boolalpha);
                        if ((engine.storedData->DataModeType() & DATA_MODE_DEPENDENT) != 0U)
                        {
                            for (std::size_t idx = engine.Length() - 1; idx != 0; --idx) {
                                stream << engine.storedData->BitsInformation().GetBitValue(idx);
                                if (idx % 8 == 0) { stream << ' '; }
                            }
                            stream << engine.storedData->BitsInformation().GetBitValue(0);
                        }
                        else  // If data handling mode type is DATA_MODE_INDEPENDENT.
                        {
                            for (std::size_t idx = 0; idx < engine.Length(); ++idx) {
                                if (idx % 8 == 0 && idx != 0) { stream << ' '; }
                                stream << engine.storedData->BitsInformation().GetBitValue(idx);
                            }
                        }
                    }
                }
                catch (const std::ios_base::failure& /*err*/) { }
                return stream;
            }


            /**
             * @brief Bitwise left shift assignment operator that performs direct left bit shift by a specified bit offset.
             *
             * @param [in] shift - Bit offset for direct left bit shift as right operand.
             * @return Lvalue reference of transformed BitStreamTransformEngine class.
             */
            BitStreamTransformEngine& operator<<= (const std::size_t shift) noexcept { return ShiftLeft(shift, false); }

            /**
             * @brief Bitwise right shift assignment operator that performs direct right bit shift by a specified bit offset.
             *
             * @param [in] shift - Bit offset for direct right bit shift as right operand.
             * @return Lvalue reference of transformed BitStreamTransformEngine class.
             */
            BitStreamTransformEngine& operator>>= (const std::size_t shift) noexcept { return ShiftRight(shift, false); }

            /**
             * @brief Logical assignment bitwise AND operator that transforms internal binary data.
             *
             * @param [in] other - Const lvalue reference of the BitStreamTransformEngine class as right operand.
             * @return Lvalue reference of transformed (by operation AND) BitStreamTransformEngine class.
             *
             * @note If operands have different data length then result data will be the length of the lesser among the operands.
             * @note If data handling mode type is DATA_MODE_SAFE_OPERATOR then the size of result data will be preserved.
             *
             * @attention The correct result can only be obtained if the data dependent modes are the same.
             */
            BitStreamTransformEngine & operator&= (const BitStreamTransformEngine & /*other*/) noexcept;

            /**
             * @brief Logical assignment bitwise AND operator that transforms internal binary data.
             *
             * @param [in] other - Const lvalue reference of the BitStreamInformationEngine class as right operand.
             * @return Lvalue reference of transformed (by operation AND) BitStreamTransformEngine class.
             *
             * @note If operands have different data length then result data will be the length of the lesser among the operands.
             * @note If data handling mode type is DATA_MODE_SAFE_OPERATOR then the size of result data will be preserved.
             *
             * @attention The correct result can only be obtained if the data dependent modes are the same.
             */
            BitStreamTransformEngine& operator&= (const BitStreamInformationEngine& other) noexcept
            {
                *this &= other.storedData->bitStreamTransform;
                return *this;
            }

            /**
             * @brief Logical assignment bitwise OR operator that transforms internal binary data.
             *
             * @param [in] other - Const lvalue reference of the BitStreamTransformEngine class as right operand.
             * @return Lvalue reference of transformed (by operation OR) BitStreamTransformEngine class.
             *
             * @note If operands have different data length then result data will be the length of the largest among the operands.
             * @note If data handling mode type is DATA_MODE_SAFE_OPERATOR then the size of result data will be preserved.
             *
             * @attention The correct result can only be obtained if the data dependent modes are the same.
             */
            BitStreamTransformEngine & operator|= (const BitStreamTransformEngine & /*other*/) noexcept;

            /**
             * @brief Logical assignment bitwise OR operator that transforms internal binary data.
             *
             * @param [in] other - Const lvalue reference of the BitStreamInformationEngine class as right operand.
             * @return Lvalue reference of transformed (by operation OR) BitStreamTransformEngine class.
             *
             * @note If operands have different data length then result data will be the length of the largest among the operands.
             * @note If data handling mode type is DATA_MODE_SAFE_OPERATOR then the size of result data will be preserved.
             *
             * @attention The correct result can only be obtained if the data dependent modes are the same.
             */
            BitStreamTransformEngine& operator|= (const BitStreamInformationEngine& other) noexcept
            {
                *this |= other.storedData->bitStreamTransform;
                return *this;
            }

            /**
             * @brief Logical assignment bitwise XOR operator that transforms internal binary data.
             *
             * @param [in] other - Const lvalue reference of the BitStreamTransformEngine class as right operand.
             * @return Lvalue reference of transformed (by operation XOR) BitStreamTransformEngine class.
             *
             * @note If operands have different data length then result data will be the length of the largest among the operands.
             * @note If data handling mode type is DATA_MODE_SAFE_OPERATOR then the length of result data will be preserved.
             *
             * @attention The correct result can only be obtained if the data dependent modes are the same.
             */
            BitStreamTransformEngine & operator^= (const BitStreamTransformEngine & /*other*/) noexcept;

            /**
             * @brief Logical assignment bitwise XOR operator that transforms internal binary data.
             *
             * @param [in] other - Const lvalue reference of the BitStreamInformationEngine class as right operand.
             * @return Lvalue reference of transformed (by operation XOR) BitStreamTransformEngine class.
             *
             * @note If operands have different data length then result data will be the length of the largest among the operands.
             * @note If data handling mode type is DATA_MODE_SAFE_OPERATOR then the length of result data will be preserved.
             *
             * @attention The correct result can only be obtained if the data dependent modes are the same.
             */
            BitStreamTransformEngine& operator^= (const BitStreamInformationEngine& other) noexcept
            {
                *this ^= other.storedData->bitStreamTransform;
                return *this;
            }

        };


        /**
         * @class ByteStreamEngine   BinaryDataEngine.hpp   "include/framework/BinaryDataEngine.hpp"
         * @brief Class that operates on a sequence of bytes and offers an interface for working with them.
         *
         * @attention This class MUST BE initialized in all constructors of owner class.
         */
        class ByteStreamEngine
        {
            friend class BinaryDataEngine;
            friend class BinaryStructuredDataEngine;

        private:
            /**
             * @brief Lvalue reference of the BinaryDataEngine owner class.
             */
            BinaryDataEngine & storedData;

            /**
             * @brief Method that returns the correct position of selected byte in stored binary data in any data endian.
             *
             * @param [in] index - Index of byte in stored binary data.
             * @return Index of element in array of binary stored data.
             *
             * @note Before using this method, MUST be checked that the index does not out-of-range.
             */
            std::size_t GetBytePosition (std::size_t /*index*/) const noexcept;

        public:
            ByteStreamEngine(void) = delete;
            ByteStreamEngine (ByteStreamEngine &&) = delete;
            ByteStreamEngine (const ByteStreamEngine &) = delete;
            ByteStreamEngine & operator= (ByteStreamEngine &&) = delete;
            ByteStreamEngine & operator= (const ByteStreamEngine &) = delete;

            /**
             * @brief Constructor of nested ByteStreamEngine class.
             * @param [in] owner - Lvalue reference of BinaryDataEngine owner class.
             */
            explicit ByteStreamEngine (BinaryDataEngine& owner) noexcept
                : storedData(owner)
            { }

            /**
             * @brief Default destructor of nested ByteStreamEngine class.
             */
            ~ByteStreamEngine(void) noexcept = default;

            /**
             * @brief Method that returns the length of stored data in bytes.
             * @return Length of byte sequence of stored data.
             */
            inline std::size_t Length(void) const noexcept { return storedData.length; }

            /**
             * @brief Method that performs direct left byte shift by a specified byte offset.
             *
             * @param [in] shift - Byte offset for direct left byte shift.
             * @param [in] fillByte - Value of the fill byte after the left shift. Default: 0x00.
             * @return Const lvalue reference of ByteStreamEngine class.
             */
            const ByteStreamEngine & ShiftLeft (std::size_t /*shift*/, std::byte /*fillByte*/ = std::byte(0x00)) const noexcept;

            /**
             * @brief Method that performs direct right byte shift by a specified byte offset.
             *
             * @param [in] shift - Byte offset for direct right byte shift.
             * @param [in] fillByte - Value of the fill byte after the right shift. Default: 0x00.
             * @return Const lvalue reference of ByteStreamEngine class.
             */
            const ByteStreamEngine & ShiftRight (std::size_t /*shift*/, std::byte /*fillByte*/ = std::byte(0x00)) const noexcept;

            /**
             * @brief Method that performs round left byte shift by a specified byte offset.
             *
             * @param [in] shift - Byte offset for round left byte shift.
             * @return Const lvalue reference of ByteStreamEngine class.
             */
            const ByteStreamEngine & RoundShiftLeft (std::size_t /*shift*/) const noexcept;

            /**
             * @brief Method that performs round right byte shift by a specified byte offset.
             *
             * @param [in] shift - Byte offset for round right byte shift.
             * @return Const lvalue reference of ByteStreamEngine class.
             */
            const ByteStreamEngine & RoundShiftRight (std::size_t /*shift*/) const noexcept;

            /**
             * @brief Method that checks the byte under the specified index.
             *
             * @param [in] index - Index of byte in stored binary data.
             * @param [in] value - Value of the pattern byte for check.
             * @return TRUE - if byte under selected index of stored data has specified value, otherwise - FALSE.
             *
             * @warning Method always returns 'false' if the index is out-of-range.
             */
            bool Test (std::size_t /*index*/, std::byte /*value*/) const noexcept;

            /**
             * @brief Method that returns byte sequence characteristic when all bytes have a specified value in block of stored data.
             *
             * @param [in] value - Value of the pattern byte for check. Default: 0xFF.
             * @param [in] first - First index of byte in binary sequence from which sequent bytes will be checked. Default: 0.
             * @param [in] last - Last index of byte in binary sequence to which (inclusive) bytes will be checked. Default: npos.
             * @return TRUE - if all bytes in block of stored data have a specified value, otherwise - FALSE.
             *
             * @warning Method always returns 'false' if the index is out-of-range.
             */
            bool All (std::byte /*value*/ = std::byte(0xFF), std::size_t /*first*/ = 0, std::size_t /*last*/ = npos) const noexcept;

            /**
             * @brief Method that returns byte sequence characteristic when any of the bytes have a specified value in block of stored data.
             *
             * @param [in] value - Value of the pattern byte for check. Default: 0xFF.
             * @param [in] first - First index of byte in binary sequence from which sequent bytes will be checked. Default: 0.
             * @param [in] last - Last index of byte in binary sequence to which (inclusive) bytes will be checked. Default: npos.
             * @return TRUE - if any of the bytes in block of stored data have a specified value, otherwise - FALSE.
             *
             * @warning Method always returns 'false' if the index is out-of-range.
             */
            bool Any (std::byte /*byte*/ = std::byte(0xFF), std::size_t /*first*/ = 0, std::size_t /*last*/ = npos) const noexcept;

            /**
             * @brief Method that returns byte sequence characteristic when none of the bytes have a specified value in block of stored data.
             *
             * @param [in] value - Value of the pattern byte for check. Default: 0x00.
             * @param [in] first - First index of byte in binary sequence from which sequent bytes will be checked. Default: 0.
             * @param [in] last - Last index of byte in binary sequence to which (inclusive) bytes will be checked. Default: npos.
             * @return TRUE - if all of the bytes in block of stored data have not a specified value, otherwise - FALSE.
             *
             * @warning Method always returns 'false' if the index is out-of-range.
             */
            bool None (std::byte /*byte*/ = std::byte(0x00), std::size_t /*first*/ = 0, std::size_t /*last*/ = npos) const noexcept;

            /**
             * @brief Operator that returns the value of byte under the specified index.
             *
             * @param [in] index - Index of byte in stored binary data.
             * @return Value of the selected byte.
             */
            inline std::optional<std::byte> operator[] (const std::size_t index) const noexcept
            {
                if (index >= Length()) { return std::nullopt; }
                return storedData.data[GetBytePosition(index)];
            }

            /**
             * @brief Method that returns a pointer to the value of byte under the specified endian-oriented byte index.
             *
             * @param [in] index - Index of byte in stored binary data.
             * @return Return a pointer to the value of byte under the specified index or nullptr if index is out-of-range.
             *
             * @note This method considers the endian type in which binary stored data are presented.
             * @note Return value is marked with the "nodiscard" attribute.
             */
            [[nodiscard]]
            std::byte * GetAt (std::size_t /*index*/) const noexcept;
        };

    private:
        /**
         * @brief Internal variable that contains binary data.
         */
        std::unique_ptr<std::byte[]> data = nullptr;
        /**
         * @brief Length of stored data in bytes.
         */
        std::size_t length = 0;
        /**
         * @brief Handling mode type of stored data.
         */
        mutable uint8_t dataModeType = DATA_MODE_DEFAULT;
        /**
         * @brief Endian type of stored data.
         */
        mutable DATA_ENDIAN_TYPE dataEndianType = system_endian;
        /**
         * @brief Transform engine for working with sequence of bits.
         */
        BitStreamTransformEngine bitStreamTransform;
        /**
         * @brief Information engine for working with sequence of bits.
         */
        BitStreamInformationEngine bitStreamInformation;
        /**
         * @brief Engine for working with sequence of bytes.
         */
        ByteStreamEngine byteStreamTransform;


    public:
        /**
         * @brief Constructor of BinaryDataEngine class.
         * @param [in] mode - Type of the data handling mode. Default: DATA_DEFAULT_MODE.
         * @param [in] endian - Endian of stored data. Default: Local System Endian Type (DATA_SYSTEM_ENDIAN).
         */
        explicit BinaryDataEngine (const uint8_t mode = DATA_MODE_DEFAULT, const DATA_ENDIAN_TYPE endian = system_endian) noexcept
            : dataModeType(mode), dataEndianType(endian),
              bitStreamTransform(this), bitStreamInformation(this),
              byteStreamTransform(*this)
        { }

        /**
         * @brief Destructor of BinaryDataEngine class.
         */
        ~BinaryDataEngine(void) noexcept { Reset(); }

        /**
         * @brief Copy assignment constructor of BinaryDataEngine class.
         * @param [in] other - Const lvalue reference of copied BinaryDataEngine class.
         *
         * @note After data assignment the data handling mode is changed to DATA_MODE_ALLOCATION.
         *
         * @attention Need to check existence of data after use this constructor.
         */
        BinaryDataEngine (const BinaryDataEngine & /*other*/) noexcept;

        /**
         * @brief Move assignment constructor of BinaryDataEngine class.
         * @param [in] other - Rvalue reference of moved BinaryDataEngine class.
         */
        BinaryDataEngine (BinaryDataEngine && /*other*/) noexcept;

        /**
         * @brief Constructor that allocates specified amount of bytes.
         * @param [in] size - Number of bytes for allocate.
         * @param [in] mode - Type of the data handling mode. Default: DATA_DEFAULT_MODE.
         * @param [in] endian - Endian of stored data. Default: Local System Endian Type (DATA_SYSTEM_ENDIAN).
         *
         * @note After data assignment the data handling mode is changed to DATA_MODE_ALLOCATION.
         *
         * @attention Need to check existence of data after use this constructor.
         */
        explicit BinaryDataEngine (std::size_t /*size*/, uint8_t /*mode*/ = DATA_MODE_DEFAULT, DATA_ENDIAN_TYPE /*endian*/ = system_endian) noexcept;

        /**
         * @brief Constructor that accepts a pointer to allocated (or static) binary data.
         * @param [in] memory - Pointer to allocated (or static) data.
         * @param [in] size - Number of bytes in data.
         * @param [in] endian - Endian of inputted data. Default: Local System Endian Type (DATA_SYSTEM_ENDIAN).
         * @param [in] mode - Type of the data handling mode. Default: DATA_DEFAULT_MODE.
         * @param [in] destruct - Flag that indicates about do need to delete an object in destructor or not. Default: false.
         *
         * @note After data initialization the data handling mode is changed to DATA_MODE_NO_ALLOCATION if flag 'destruct' is 'false'.
         *
         * @attention Use option 'destruct' only in case when this object was allocated by operator 'new' and has original type 'std::byte'.
         * @attention Need to check existence of data after use this constructor.
         */
        BinaryDataEngine (std::byte *      /*memory*/,
                          std::size_t      /*size*/,
                          DATA_ENDIAN_TYPE /*endian*/   = system_endian,
                          uint8_t          /*mode*/     = DATA_MODE_DEFAULT,
                          bool             /*destruct*/ = false) noexcept;

        /**
         * @brief Constructor that accepts a pointer to const allocated (or static) binary data.
         * @param [in] memory - Pointer to const allocated (or static) data.
         * @param [in] size - Number of bytes in data.
         * @param [in] endian - Endian of inputted data. Default: Local System Endian Type (DATA_SYSTEM_ENDIAN).
         * @param [in] mode - Type of the data handling mode. Default: DATA_DEFAULT_MODE.
         *
         * @attention Need to check existence of data after use this constructor.
         */
        BinaryDataEngine (const std::byte * /*memory*/,
                          std::size_t       /*size*/,
                          DATA_ENDIAN_TYPE  /*endian*/  = system_endian,
                          uint8_t           /*mode*/    = DATA_MODE_DEFAULT) noexcept;

        /**
         * @brief Copy assignment operator of BinaryDataEngine class.
         * @param [in] other - Const lvalue reference of copied BinaryDataEngine class.
         * @return Lvalue reference of copied BinaryDataEngine class.
         *
         * @note After data assignment the data handling mode is changed to DATA_MODE_ALLOCATION.
         *
         * @attention Need to check existence of data after use this operator.
         */
        BinaryDataEngine & operator= (const BinaryDataEngine & /*other*/) noexcept;

        /**
         * @brief Move assignment operator of BinaryDataEngine class.
         * @param [in] other - Rvalue reference of moved BinaryDataEngine class.
         * @return Lvalue reference of moved BinaryDataEngine class.
         */
        BinaryDataEngine & operator= (BinaryDataEngine && /*other*/) noexcept;

        /**
         * @brief Method that assigns data to BinaryDataEngine.
         * @tparam [in] Type - Typename of assigned data.
         * @param [in] memory - Pointer to the constant data of selected type.
         * @param [in] count - The number of elements in the input data of selected type.
         * @return TRUE - if data assignment is successful, otherwise - FALSE.
         *
         * @note After data assignment the data handling mode is changed to DATA_MODE_ALLOCATION.
         * @note Input type MUST be a POD type.
         * @note Return value is marked with the "nodiscard" attribute.
         */
        template <typename Type>
        [[nodiscard]]
        bool AssignData (const Type* memory, const std::size_t count) noexcept
        {
            static_assert(common::is_pod_type<Type>::value == true, "It is not possible to use not POD type for this method.");
            if (memory == nullptr) { return false; }

            const std::size_t bytes = count * sizeof(Type);  // Calculate the number of bytes in input data.
            if (length != bytes)  // Small optimization when any data already exists.
            {
                data = system::allocMemoryForArray<std::byte>(bytes, memory, bytes);
                if (data == nullptr) { return false; }
                SetDataModeType(DATA_MODE_ALLOCATION);
                length = bytes;
            }
            else { memcpy(data.get(), memory, length); }
            return true;
        }

        /**
         * @brief Method that assigns data to BinaryDataEngine.
         * @tparam [in] Type - Typename of iterator of assigned data.
         * @param [in] begin - Iterator to the first element of const data of selected type.
         * @param [in] end - Iterator to the element following the last one of const data of selected type.
         * @return TRUE - if data assignment is successful, otherwise - FALSE.
         *
         * @note After data assignment the data handling mode is changed to DATA_MODE_ALLOCATION.
         * @note Input type under iterator MUST be a POD type.
         * @note Return value is marked with the "nodiscard" attribute.
         */
        template <typename Type>
        [[nodiscard]]
        bool AssignData (const Type begin, const Type end) noexcept
        {
            static_assert(common::is_iterator_type<Type>::value == true &&
                          common::is_pod_type<typename std::iterator_traits<Type>::value_type>::value == true,
                          "It is not possible to use not Iterator type for this method.");

            // Calculate the number of bytes in input data.
            const std::size_t bytes = static_cast<std::size_t>(std::distance(begin, end)) * sizeof(typename std::iterator_traits<Type>::value_type);
            if (length != bytes)  // Small optimization when any data already exists.
            {
                data = system::allocMemoryForArray<std::byte>(bytes, &(*begin), bytes);
                if (data == nullptr) { return false; }
                SetDataModeType(DATA_MODE_ALLOCATION);
                length = bytes;
            }
            else { memcpy(data.get(), &(*begin), length); }
            return true;
        }

        /**
         * @brief Method that accepts a pointer to allocated (or static) binary data.
         * @param [in] memory - Pointer to allocated (or static) data.
         * @param [in] size - Number of bytes in data.
         * @param [in] destruct - Flag that indicates about do need to delete an object in destructor or not. Default: false.
         *
         * @note After data initialization the data handling mode is changed to DATA_MODE_NO_ALLOCATION if flag 'destruct' is 'false'.
         *
         * @attention Use option 'destruct' only in case when this object was allocated by operator 'new' and has original type 'std::byte'.
         * @attention Need to check existence of data after use this constructor.
         */
        bool AssignReference (std::byte * /*memory*/, std::size_t /*size*/, bool /*destruct*/ = false) noexcept;

        /**
         * @brief Method that returns lvalue reference of the nested BitStreamTransformEngine class for working with bits.
         * @return Lvalue reference of the BitStreamTransformEngine class.
         */
        BitStreamTransformEngine& BitsTransform(void) noexcept { return bitStreamTransform; }

        /**
         * @brief Method that returns const lvalue reference of the nested BitStreamInformationEngine class for working with bits.
         * @return Const lvalue reference of the BitStreamInformationEngine class.
         */
        const BitStreamInformationEngine& BitsInformation(void) const noexcept { return bitStreamInformation; }

        /**
         * @brief Method that returns lvalue reference of the nested ByteStreamEngine class for working with bytes.
         * @return Lvalue reference of the ByteStreamEngine class.
         */
        ByteStreamEngine& BytesTransform(void) noexcept { return byteStreamTransform; }

        /**
         * @brief Method that returns the size of stored data.
         * @return Size of stored data in bytes.
         */
        inline std::size_t Size(void) const noexcept { return length; }

        /**
         * @brief Method that returns the pointer to the const internal data.
         * @return Pointer to the const internal data.
         *
         * @note This method does not consider the type of endian in which stored data are presented.
         */
        inline const std::byte* Data(void) const noexcept { return data.get(); }

        /**
         * @brief Method that returns the state of stored binary data in BinaryDataEngine class.
         * @return TRUE - if stored data is empty, otherwise - FALSE.
         */
        inline bool IsEmpty(void) const noexcept { return length == 0; }

        /**
         * @brief Method that returns the initialize state of stored data in BinaryDataEngine class.
         * @return TRUE - if stored data is initialized, otherwise - FALSE.
         */
        inline bool IsInitialized(void) const noexcept { return data != nullptr; }

        /**
         * @brief Method that returns the data handling mode type of stored binary data in BinaryDataEngine class.
         * @return The set of enabled modes of DATA_HANDLING_MODE enum.
         */
        inline uint8_t DataModeType(void) const noexcept { return dataModeType; }

        /**
         * @brief Method that returns the endian type of stored data in BinaryDataEngine class.
         * @return DATA_LITTLE_ENDIAN(0x02) - if on the system little endian, otherwise - DATA_BIG_ENDIAN(0x01).
         */
        inline DATA_ENDIAN_TYPE DataEndianType(void) const noexcept { return dataEndianType; }

        /**
         * @brief Method that changes handling mode type of stored data in BinaryDataEngine class.
         * @param [in] mode - New data handling mode type.
         *
         * @note Changed only the method of processing data. Data representation does not changes.
         * @note After setting a new mode then the opposite mode will be automatically turned off.
         */
        void SetDataModeType (uint8_t /*mode*/) const noexcept;

        /**
         * @brief Method that changes endian type of stored data in BinaryDataEngine class.
         * @param [in] endian - New data endian type.
         * @param [in] convert - Flag indicating whether to change the presentation of the stored data or not. Default: true.
         */
        void SetDataEndianType (DATA_ENDIAN_TYPE /*endian*/, bool /*convert*/ = true) const noexcept;

        /**
         * @brief Method that returns the data dependent mode type.
         * @return TRUE - if the data handling mode consist of the DATA_MODE_DEPENDENT flag, otherwise - FALSE.
         */
        inline bool IsDependentDataMode(void) const noexcept { return ((dataModeType & DATA_MODE_DEPENDENT) != 0U); }

        /**
         * @brief Method that returns the binary operators behavior type when transformed the stored data.
         * @return TRUE - if the data handling mode consist of the DATA_MODE_SAFE_OPERATOR flag, otherwise - FALSE.
         */
        inline bool IsSafeOperatorDataMode(void) const noexcept { return ((dataModeType & DATA_MODE_SAFE_OPERATOR) != 0U); }

        /**
         * @brief Method that returns the data allocation mode type.
         * @return TRUE - if the data handling mode consist of the DATA_MODE_ALLOCATION flag, otherwise - FALSE.
         */
        inline bool IsAllocationDataMode(void) const noexcept { return ((dataModeType & DATA_MODE_ALLOCATION) != 0U); }

        /**
         * @brief Method that returns the binary operators behavior type when transformed the stored data.
         * @return TRUE - if the data handling mode consist of the DATA_MODE_OPERATOR_ALIGN_LOW_ORDER flag, otherwise - FALSE.
         */
        inline bool IsOperatorAlignLowOrderDataMode(void) const noexcept { return ((dataModeType & DATA_MODE_OPERATOR_ALIGN_LOW_ORDER) != 0U); }

        /**
         * @brief Method that returns a pointer to an element by selected index.
         * @param [in] index - Index of byte in byte sequence of stored data.
         * @return Return a pointer to an element by selected index or nullptr in an error occurred.
         *
         * @note This method does not consider the endian type in which stored data are presented.
         * @note Return value is marked with the "nodiscard" attribute.
         */
        [[nodiscard]]
        std::byte * GetAt (std::size_t /*index*/) const noexcept;

        /**
         * @brief Method that clears the internal binary data.
         */
        void Clear(void) noexcept;

        /**
         * @brief Method that resets the internal state of BinaryDataEngine class to default state.
         */
        void Reset(void) noexcept;

        /**
         * @brief Method that returns internal binary data represented in hex string.
         * @return String that represent internal binary data.
         */
        std::string ToHexString(void) const noexcept;

        /**
         * @brief Operator that returns the internal state of BinaryDataEngine class.
         * @return TRUE - if BinaryDataEngine class is not empty, otherwise - FALSE.
         */
        inline operator bool(void) const noexcept { return (data != nullptr && length != 0); }

        /**
         * @brief Operator that returns a const reference to an element by selected index.
         * @param [in] index - Index of byte in byte sequence of stored data.
         * @return Return a const reference to an element by selected index.
         *
         * @note This method does not consider the type of endian in which data are presented.
         */
        std::optional<std::byte> operator[] (std::size_t /*index*/) const noexcept;

        /**
         * @brief Logical assignment bitwise AND operator that transforms internal binary data.
         * @param [in] other - Const lvalue reference of the BinaryDataEngine class as right operand.
         * @return Lvalue reference of transformed (by operation AND) BinaryDataEngine class.
         *
         * @note If operands have different data length then result data will be the length of the lesser among the operands.
         * @note If data handling mode type is DATA_MODE_SAFE_OPERATOR then the size of result data will be preserved.
         *
         * @attention The correct result can only be obtained if the data dependent modes are the same.
         */
        BinaryDataEngine & operator&= (const BinaryDataEngine & /*other*/) noexcept;

        /**
         * @brief Logical assignment bitwise OR operator that transforms internal binary data.
         * @param [in] other - Const lvalue reference of the BinaryDataEngine class as right operand.
         * @return Lvalue reference of transformed (by operation OR) BinaryDataEngine class.
         *
         * @note If operands have different data length then result data will be the length of the largest among the operands.
         * @note If data handling mode type is DATA_MODE_SAFE_OPERATOR then the size of result data will be preserved.
         *
         * @attention The correct result can only be obtained if the data dependent modes are the same.
         */
        BinaryDataEngine & operator|= (const BinaryDataEngine & /*other*/) noexcept;

        /**
         * @brief Logical assignment bitwise XOR operator that transforms internal binary data.
         * @param [in] other - Const lvalue reference of the BinaryDataEngine class as right operand.
         * @return Lvalue reference of transformed (by operation XOR) BinaryDataEngine class.
         *
         * @note If operands have different data length then result data will be the length of the largest among the operands.
         * @note If data handling mode type is DATA_MODE_SAFE_OPERATOR then the length of result data will be preserved.
         *
         * @attention The correct result can only be obtained if the data dependent modes are the same.
         */
        BinaryDataEngine & operator^= (const BinaryDataEngine & /*other*/) noexcept;



        /**
         * @brief Logical bitwise AND operator that transforms internal binary data.
         * @param [in] left - Const lvalue reference of the BitStreamTransformEngine class as left operand.
         * @param [in] right - Const lvalue reference of the BitStreamTransformEngine class as right operand.
         * @return New temporary object of transformed (by operation AND) BinaryDataEngine class.
         *
         * @note If operands have different endian and mode then result BinaryDataEngine will have endian and mode of the left operand.
         * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LESSER length.
         *
         * @attention Result BinaryDataEngine class is always processing in data mode type of left operand.
         * @attention The correct result can only be obtained if the data dependent modes are the same.
         */
        friend inline BinaryDataEngine operator& (const BitStreamTransformEngine& left, const BitStreamTransformEngine& right) noexcept
        {
            BinaryDataEngine result(*left.storedData);
            if (result == true) {
                result.BitsTransform() &= right;
            }
            return result;
        }

        /**
         * @brief Logical bitwise AND operator that transforms internal binary data.
         * @param [in] left - Const lvalue reference of the BitStreamInformationEngine class as left operand.
         * @param [in] right - Const lvalue reference of the BitStreamInformationEngine class as right operand.
         * @return New temporary object of transformed (by operation AND) BinaryDataEngine class.
         *
         * @note If operands have different endian and mode then result BinaryDataEngine will have endian and mode of the left operand.
         * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LESSER length.
         *
         * @attention Result BinaryDataEngine class is always processing in data mode type of left operand.
         * @attention The correct result can only be obtained if the data dependent modes are the same.
         */
        friend inline BinaryDataEngine operator& (const BitStreamInformationEngine& left, const BitStreamInformationEngine& right) noexcept
        {
            BinaryDataEngine result(*left.storedData);
            if (result == true) {
                result.BitsTransform() &= right;
            }
            return result;
        }

        /**
         * @brief Logical bitwise OR operator that transforms internal binary data.
         * @param [in] left - Const lvalue reference of the BitStreamTransformEngine class as left operand.
         * @param [in] right - Const lvalue reference of the BitStreamTransformEngine class as right operand.
         * @return New temporary object of transformed (by operation OR) BinaryDataEngine class.
         *
         * @note If operands have different endian and mode then result BinaryDataEngine will have endian and mode of the left operand.
         * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LONGER length.
         *
         * @attention Result BinaryDataEngine class is always processing in data mode type of left operand.
         * @attention The correct result can only be obtained if the data dependent modes are the same.
         */
        friend inline BinaryDataEngine operator| (const BitStreamTransformEngine& left, const BitStreamTransformEngine& right) noexcept
        {
            BinaryDataEngine result(*left.storedData);
            if (result == true) {
                result.BitsTransform() |= right;
            }
            return result;
        }

        /**
         * @brief Logical bitwise OR operator that transforms internal binary data.
         * @param [in] left - Const lvalue reference of the BitStreamInformationEngine class as left operand.
         * @param [in] right - Const lvalue reference of the BitStreamInformationEngine class as right operand.
         * @return New temporary object of transformed (by operation OR) BinaryDataEngine class.
         *
         * @note If operands have different endian and mode then result BinaryDataEngine will have endian and mode of the left operand.
         * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LONGER length.
         *
         * @attention Result BinaryDataEngine class is always processing in data mode type of left operand.
         * @attention The correct result can only be obtained if the data dependent modes are the same.
         */
        friend inline BinaryDataEngine operator| (const BitStreamInformationEngine& left, const BitStreamInformationEngine& right) noexcept
        {
            BinaryDataEngine result(*left.storedData);
            if (result == true) {
                result.BitsTransform() |= right;
            }
            return result;
        }

        /**
         * @brief Logical bitwise XOR operator that transforms internal binary data.
         * @param [in] left - Const lvalue reference of the BitStreamTransformEngine class as left operand.
         * @param [in] right - Const lvalue reference of the BitStreamTransformEngine class as right operand.
         * @return New temporary object of transformed (by operation XOR) BinaryDataEngine class.
         *
         * @note If operands have different endian and mode then result BinaryDataEngine will have endian and mode of the left operand.
         * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LONGER length.
         *
         * @attention Result BinaryDataEngine class is always processing in data mode type of left operand.
         * @attention The correct result can only be obtained if the data dependent modes are the same.
         */
        friend inline BinaryDataEngine operator^ (const BitStreamTransformEngine& left, const BitStreamTransformEngine& right) noexcept
        {
            BinaryDataEngine result(*left.storedData);
            if (result == true) {
                result.BitsTransform() ^= right;
            }
            return result;
        }

        /**
         * @brief Logical bitwise XOR operator that transforms internal binary data.
         * @param [in] left - Const lvalue reference of the BitStreamInformationEngine class as left operand.
         * @param [in] right - Const lvalue reference of the BitStreamInformationEngine class as right operand.
         * @return New temporary object of transformed (by operation XOR) BinaryDataEngine class.
         *
         * @note If operands have different endian and mode then result BinaryDataEngine will have endian and mode of the left operand.
         * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LONGER length.
         *
         * @attention Result BinaryDataEngine class is always processing in data mode type of left operand.
         * @attention The correct result can only be obtained if the data dependent modes are the same.
         */
        friend inline BinaryDataEngine operator^ (const BitStreamInformationEngine& left, const BitStreamInformationEngine& right) noexcept
        {
            BinaryDataEngine result(*left.storedData);
            if (result == true) {
                result.BitsTransform() ^= right;
            }
            return result;
        }

        /**
         * @brief Bitwise left shift operator that performs direct left bit shift by a specified bit offset.
         * @param [in] engine - Const lvalue reference of the BitStreamTransformEngine class as left operand.
         * @param [in] shift - Bit offset for direct left bit shift as right operand.
         * @return New temporary object of transformed BinaryDataEngine class.
         */
        friend inline BinaryDataEngine operator<< (const BitStreamTransformEngine& engine, const std::size_t shift) noexcept
        {
            BinaryDataEngine result(*engine.storedData);
            result.BitsTransform().ShiftLeft(shift, false);
            return result;
        }

        /**
         * @brief Bitwise right shift operator that performs direct right bit shift by a specified bit offset.
         * @param [in] engine - Const lvalue reference of the BitStreamTransformEngine class as left operand.
         * @param [in] shift - Bit offset for direct right bit shift as right operand.
         * @return New temporary object of transformed BinaryDataEngine class.
         */
        friend inline BinaryDataEngine operator>> (const BitStreamTransformEngine& engine, const std::size_t shift) noexcept
        {
            BinaryDataEngine result(*engine.storedData);
            result.BitsTransform().ShiftRight(shift, false);
            return result;
        }

        /**
         * @brief Logical bitwise complement operator that inverts each bit in internal binary data.
         * @param [in] engine - Const lvalue reference of the BitStreamTransformEngine class.
         * @return New temporary object of transformed (by operation NOT) BinaryDataEngine class.
         */
        friend inline BinaryDataEngine operator~ (const BitStreamTransformEngine& engine) noexcept
        {
            BinaryDataEngine result(*engine.storedData);
            result.BitsTransform().InvertBlock();
            return result;
        }

    };



    /**
     * @brief Logical bitwise AND operator that transforms internal binary data.
     * @param [in] left - Const lvalue reference of the BinaryDataEngine class as left operand.
     * @param [in] right - Const lvalue reference of the BinaryDataEngine class as right operand.
     * @return New temporary object of transformed (by operation AND) BinaryDataEngine class.
     *
     * @note If operands have different endian and mode then result BinaryDataEngine will have endian and mode of the left operand.
     * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LESSER length (DATA_MODE_UNSAFE_OPERATOR mode).
     *
     * @attention Result BinaryDataEngine class is always processing in data mode type of left operand.
     * @attention The correct result can only be obtained if the data dependent modes are the same.
     */
    inline BinaryDataEngine operator& (const BinaryDataEngine& left, const BinaryDataEngine& right) noexcept
    {
        BinaryDataEngine result(left);
        if (result == true) {
            result.BitsTransform() &= right.BitsInformation();
        }
        return result;
    }

    /**
     * @brief Logical bitwise OR operator that transforms internal binary data.
     * @param [in] left - Const lvalue reference of the BinaryDataEngine class as left operand.
     * @param [in] right - Const lvalue reference of the BinaryDataEngine class as right operand.
     * @return New temporary object of transformed (by operation OR) BinaryDataEngine class.
     *
     * @note If operands have different endian and mode then result BinaryDataEngine will have endian and mode of the left operand.
     * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LONGER length (DATA_MODE_UNSAFE_OPERATOR mode).
     *
     * @attention Result BinaryDataEngine class is always processing in data mode type of left operand.
     * @attention The correct result can only be obtained if the data dependent modes are the same.
     */
    inline BinaryDataEngine operator| (const BinaryDataEngine& left, const BinaryDataEngine& right) noexcept
    {
        BinaryDataEngine result(left);
        if (result == true) {
            result.BitsTransform() |= right.BitsInformation();
        }
        return result;
    }

    /**
     * @brief Logical bitwise XOR operator that transforms internal binary data.
     * @param [in] left - Const lvalue reference of the BinaryDataEngine class as left operand.
     * @param [in] right - Const lvalue reference of the BinaryDataEngine class as right operand.
     * @return New temporary object of transformed (by operation XOR) BinaryDataEngine class.
     *
     * @note If operands have different endian and mode then result BinaryDataEngine will have endian and mode of the left operand.
     * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LONGER length (DATA_MODE_UNSAFE_OPERATOR mode).
     *
     * @attention Result BinaryDataEngine class is always processing in data mode type of left operand.
     * @attention The correct result can only be obtained if the data dependent modes are the same.
     */
    inline BinaryDataEngine operator^ (const BinaryDataEngine& left, const BinaryDataEngine& right) noexcept
    {
        BinaryDataEngine result(left);
        if (result == true) {
            result.BitsTransform() ^= right.BitsInformation();
        }
        return result;
    }

}  // namespace types.


#endif  // PROTOCOL_ANALYZER_BINARY_DATA_ENGINE_HPP
