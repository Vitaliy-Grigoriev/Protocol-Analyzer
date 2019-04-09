// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
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

// ////////////// DATA ENDIAN TYPE ////////////////
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
//  Another endian models (not supported).
//
//  Middle Endian Big Model. (PDP-11)
//  |23______16|31______24|7______0|15______8|
//
//  Middle Endian Little Model. (Honeywell 316)
//  |15______8|7______0|31______24|23______16|
//
// ////////////////////////////////////////////////

/**
 * @defgroup BYTE_CONSTANTS   Byte defines.
 * @brief This group of constants defines all bytes which are used in BinaryDataEngine class.
 * @{
 */
inline constexpr std::byte HighByte = std::byte(0xFF);
inline constexpr std::byte LowByte = std::byte(0x00);

inline constexpr std::byte HighPartByte = std::byte(0xF0);
inline constexpr std::byte LowPartByte = std::byte(0x0F);

inline constexpr std::byte HighAlternateByte = std::byte(0xAA);
inline constexpr std::byte LowAlternateByte = std::byte(0x55);

inline constexpr std::byte HighBitsInHalvesByte = std::byte(0xCC);
inline constexpr std::byte LowBitsInHalvesByte = std::byte(0x33);

inline constexpr std::byte HighBitInByte = std::byte(0x80);
inline constexpr std::byte LowBitInByte = std::byte(0x01);
/** @} */


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
        DATA_BIG_ENDIAN = 0x01,     // First byte of the multibyte data-type is stored first.
        DATA_LITTLE_ENDIAN = 0x02,  // Last byte of the multibyte data-type is stored first.
        DATA_REVERSE_BIG_ENDIAN = 0x03,  // First byte of the multibyte data-type is stored first in reverse bit sequence.
        DATA_SYSTEM_ENDIAN = 0xFE,  // This endian type determine the system endian type and using only in constructors.
        DATA_NO_ENDIAN = 0xFF       // This endian type means that endian value does not set.
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
     * @class BinaryStructuredDataEngineBase   BinaryStructuredDataEngine.hpp   "include/framework/BinaryStructuredDataEngine.hpp"
     * @brief Forward declaration of BinaryStructuredDataEngineBase class.
     */
    template <typename BinaryDataType>
    class BinaryStructuredDataEngineBase;

    /**
     * @class IntegerValue   IntegerValue.hpp   "include/framework/IntegerValue.hpp"
     * @brief Forward declaration of IntegerValue class.
     */
    class IntegerValue;


    /**
     * @class BinaryDataEngine   BinaryDataEngine.hpp   "include/framework/BinaryDataEngine.hpp"
     * @brief Main class of analyzer framework that contains binary data and gives an interface to work with it.
     *
     * @note This class is cross-platform.
     */
    class BinaryDataEngine
    {
        friend class IntegerValue;

        template <typename BinaryDataType>
        friend class BinaryStructuredDataEngineBase;

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
         * @struct BitStreamTransformEngine   BinaryDataEngine.hpp   "include/framework/BinaryDataEngine.hpp"
         * @brief Forward declaration of BitStreamTransformEngine structure.
         */
        class BitStreamTransformEngine;

        /**
         * @struct ByteStreamEngine   BinaryDataEngine.hpp   "include/framework/BinaryDataEngine.hpp"
         * @brief Forward declaration of ByteStreamEngine structure.
         */
        class ByteStreamTransformEngine;

        /**
         * @struct ByteStreamInformationEngine   BinaryDataEngine.hpp   "include/framework/BinaryDataEngine.hpp"
         * @brief Forward declaration of ByteStreamInformationEngine structure.
         */
        class ByteStreamInformationEngine;


        /**
         * @class BitStreamInformationEngine   BinaryDataEngine.hpp   "include/framework/BinaryDataEngine.hpp"
         * @brief Class that operates on a sequence of bits and offers an interface for working with them.
         *
         * @attention This class MUST BE initialized in all constructors of owner class.
         */
        class BitStreamInformationEngine
        {
            friend class IntegerValue;
            friend class BinaryDataEngine;
            friend class BitStreamTransformEngine;
            friend class ByteStreamEngine;
            friend class ByteStreamInformationEngine;

            template <typename BinaryDataType>
            friend class BinaryStructuredDataEngineBase;

        private:
            /**
             * @brief Constant pointer to the constant BinaryDataEngine owner class.
             */
            const BinaryDataEngine * const storedData = nullptr;

            /**
             * @brief Method that returns the correct position of byte which store the selected bit under specified index in stored binary data in any data endian.
             *
             * @param [in] index - Index of bit in stored binary data.
             * @return Index of byte of binary stored data.
             *
             * @attention Before using this method, MUST be checked that the index does not out-of-range.
             */
            std::size_t GetByteIndex (std::size_t /*index*/) const noexcept;

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
             * @param [in] owner - Constant pointer to the const BinaryDataEngine owner class.
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
             * @brief Method that returns the constant reference to the BinaryDataEngine owning object.
             *
             * @return Constant reference to the BinaryDataEngine object.
             */
            inline const BinaryDataEngine& DataEngine(void) const noexcept { return *storedData; }

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
             * @brief Method that converts interval of stored binary data into user type.
             *
             * @tparam [in] Type - Typename of variable to which stored data will be converted.
             * @tparam [in] Size - Number of bytes in output data (Used only if Type is a compound variable).
             * @tparam [in] Endian - Endian of output data. Default: Local System Type (DATA_SYSTEM_ENDIAN).
             * @param [in] first - First index of bit in binary sequence from which sequent bits will be copied. Default: 0.
             * @param [in] last - Last index of bit in binary sequence to which previous bits will be copied. Default: npos.
             * @return Variable of selected type that consist of the bit sequence in the selected interval of stored data.
             *
             * @attention If output real data size (in case structure/class) less then sizeof(Type) then MUST specify their size.
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
                    wrapper.BitsTransform().Set(position++, storedData->bitStreamInformation.GetBitValue(first++));
                }
                return result;
            }

            /**
             * @brief Operator that outputs internal binary data in binary string format.
             *
             * @param [in,out] stream - Reference of the output stream engine.
             * @param [in] engine - Constant lvalue reference of the BitStreamTransformEngine class.
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

            /**
             * @brief Bitwise AND operator.
             *
             * @param [in] left - Constant lvalue reference of the BitStreamInformationEngine class as left operand.
             * @param [in] right - Constant lvalue reference of the BitStreamInformationEngine class as right operand.
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
                BinaryDataEngine result(left.DataEngine());
                if (result == true) {
                    result.BitsTransform() &= right;
                }
                return result;
            }

            /**
             * @brief Bitwise AND operator.
             *
             * @param [in] left - Constant lvalue reference of the BitStreamInformationEngine class as left operand.
             * @param [in] right - Constant lvalue reference of the BitStreamTransformEngine class as right operand.
             * @return New temporary object of transformed (by operation AND) BinaryDataEngine class.
             *
             * @note If operands have different endian and mode then result BinaryDataEngine will have endian and mode of the left operand.
             * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LESSER length.
             *
             * @attention Result BinaryDataEngine class is always processing in data mode type of left operand.
             * @attention The correct result can only be obtained if the data dependent modes are the same.
             */
            friend inline BinaryDataEngine operator& (const BitStreamInformationEngine& left, const BitStreamTransformEngine& right) noexcept
            {
                BinaryDataEngine result(left.DataEngine());
                if (result == true) {
                    result.BitsTransform() &= right;
                }
                return result;
            }

            /**
             * @brief Bitwise OR operator.
             *
             * @param [in] left - Constant lvalue reference of the BitStreamInformationEngine class as left operand.
             * @param [in] right - Constant lvalue reference of the BitStreamInformationEngine class as right operand.
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
                BinaryDataEngine result(left.DataEngine());
                if (result == true)
                {
                    result.BitsTransform() |= right;
                }
                return result;
            }

            /**
             * @brief Bitwise OR operator.
             *
             * @param [in] left - Constant lvalue reference of the BitStreamInformationEngine class as left operand.
             * @param [in] right - Constant lvalue reference of the BitStreamTransformEngine class as right operand.
             * @return New temporary object of transformed (by operation OR) BinaryDataEngine class.
             *
             * @note If operands have different endian and mode then result BinaryDataEngine will have endian and mode of the left operand.
             * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LONGER length.
             *
             * @attention Result BinaryDataEngine class is always processing in data mode type of left operand.
             * @attention The correct result can only be obtained if the data dependent modes are the same.
             */
            friend inline BinaryDataEngine operator| (const BitStreamInformationEngine& left, const BitStreamTransformEngine& right) noexcept
            {
                BinaryDataEngine result(left.DataEngine());
                if (result == true) {
                    result.BitsTransform() |= right;
                }
                return result;
            }

            /**
             * @brief Bitwise XOR operator.
             *
             * @param [in] left - Constant lvalue reference of the BitStreamInformationEngine class as left operand.
             * @param [in] right - Constant lvalue reference of the BitStreamInformationEngine class as right operand.
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
                BinaryDataEngine result(left.DataEngine());
                if (result == true) {
                    result.BitsTransform() ^= right;
                }
                return result;
            }

            /**
             * @brief Bitwise XOR operator.
             *
             * @param [in] left - Constant lvalue reference of the BitStreamInformationEngine class as left operand.
             * @param [in] right - Constant lvalue reference of the BitStreamTransformEngine class as right operand.
             * @return New temporary object of transformed (by operation XOR) BinaryDataEngine class.
             *
             * @note If operands have different endian and mode then result BinaryDataEngine will have endian and mode of the left operand.
             * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LONGER length.
             *
             * @attention Result BinaryDataEngine class is always processing in data mode type of left operand.
             * @attention The correct result can only be obtained if the data dependent modes are the same.
             */
            friend inline BinaryDataEngine operator^ (const BitStreamInformationEngine& left, const BitStreamTransformEngine& right) noexcept
            {
                BinaryDataEngine result(left.DataEngine());
                if (result == true) {
                    result.BitsTransform() ^= right;
                }
                return result;
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
            friend class IntegerValue;
            friend class BinaryDataEngine;
            friend class BitStreamInformationEngine;
            friend class ByteStreamEngine;
            friend class ByteStreamInformationEngine;

            template <typename BinaryDataType>
            friend class BinaryStructuredDataEngineBase;

        private:
            /**
             * @brief Constant pointer to the BinaryDataEngine owner class.
             */
            BinaryDataEngine * const storedData = nullptr;

        public:
            BitStreamTransformEngine(void) = delete;
            BitStreamTransformEngine (BitStreamTransformEngine &&) = delete;
            BitStreamTransformEngine (const BitStreamTransformEngine &) = delete;
            BitStreamTransformEngine & operator= (BitStreamTransformEngine &&) = delete;
            BitStreamTransformEngine & operator= (const BitStreamTransformEngine &) = delete;

            /**
             * @brief Constructor of nested BitStreamTransformEngine class.
             *
             * @param [in] owner - Constant pointer to the BinaryDataEngine owner class.
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
             * @brief Method that returns the reference to the BinaryDataEngine owning object.
             *
             * @return Reference to the BinaryDataEngine object.
             */
            inline BinaryDataEngine& DataEngine(void) const noexcept { return *storedData; }

            /**
             * @brief Method that performs direct left bit shift by a specified bit offset.
             *
             * @param [in] shift - Bit offset for direct left bit shift.
             * @param [in] fillBit - Value of the fill bit after the left shift. Default: false (0).
             * @return Constant lvalue reference of BitStreamTransformEngine class.
             */
            BitStreamTransformEngine & ShiftLeft (std::size_t /*shift*/, bool /*fillBit*/ = false) noexcept;

            /**
             * @brief Method that performs direct right bit shift by a specified bit offset.
             *
             * @param [in] shift - Bit offset for direct right bit shift.
             * @param [in] fillBit - Value of the fill bit after the right shift. Default: false (0).
             * @return Constant lvalue reference of BitStreamTransformEngine class.
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
             * @return Constant lvalue reference of modified BitStreamTransformEngine class.
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
             * @return Constant lvalue reference of BitStreamTransformEngine class.
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
             * @brief Operator that outputs internal binary data in binary string format.
             *
             * @param [in,out] stream - Reference of the output stream engine.
             * @param [in] engine - Constant lvalue reference of the BitStreamTransformEngine class.
             * @return Lvalue reference of the inputted STL std::ostream class.
             *
             * @note Data is always outputs in DATA_BIG_ENDIAN endian type if data handling mode type is DATA_MODE_DEPENDENT.
             */
            friend inline std::ostream& operator<< (std::ostream& stream, const BitStreamTransformEngine& engine) noexcept
            {
                stream << engine.storedData->BitsInformation();
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
             * @brief Bitwise assignment AND operator.
             *
             * @param [in] other - Constant lvalue reference of the BitStreamTransformEngine class as right operand.
             * @return Lvalue reference of transformed (by operation AND) BitStreamTransformEngine class.
             *
             * @note If operands have different data length then result data will be the length of the lesser among the operands.
             * @note If data handling mode type is DATA_MODE_SAFE_OPERATOR then the size of result data will be saved.
             *
             * @attention The correct result can only be obtained if the data dependent modes are the same.
             */
            BitStreamTransformEngine & operator&= (const BitStreamTransformEngine & /*other*/) noexcept;

            /**
             * @brief Bitwise assignment AND operator.
             *
             * @param [in] other - Constant lvalue reference of the BitStreamInformationEngine class as right operand.
             * @return Lvalue reference of transformed (by operation AND) BitStreamTransformEngine class.
             *
             * @note If operands have different data length then result data will be the length of the lesser among the operands.
             * @note If data handling mode type is DATA_MODE_SAFE_OPERATOR then the size of result data will be saved.
             *
             * @attention The correct result can only be obtained if the data dependent modes are the same.
             */
            BitStreamTransformEngine& operator&= (const BitStreamInformationEngine& other) noexcept
            {
                *this &= other.storedData->bitStreamTransform;
                return *this;
            }

            /**
             * @brief Bitwise assignment OR operator.
             *
             * @param [in] other - Constant lvalue reference of the BitStreamTransformEngine class as right operand.
             * @return Lvalue reference of transformed (by operation OR) BitStreamTransformEngine class.
             *
             * @note If operands have different data length then result data will be the length of the largest among the operands.
             * @note If data handling mode type is DATA_MODE_SAFE_OPERATOR then the size of result data will be saved.
             *
             * @attention The correct result can only be obtained if the data dependent modes are the same.
             */
            BitStreamTransformEngine & operator|= (const BitStreamTransformEngine & /*other*/) noexcept;

            /**
             * @brief Bitwise assignment OR operator.
             *
             * @param [in] other - Constant lvalue reference of the BitStreamInformationEngine class as right operand.
             * @return Lvalue reference of transformed (by operation OR) BitStreamTransformEngine class.
             *
             * @note If operands have different data length then result data will be the length of the largest among the operands.
             * @note If data handling mode type is DATA_MODE_SAFE_OPERATOR then the size of result data will be saved.
             *
             * @attention The correct result can only be obtained if the data dependent modes are the same.
             */
            BitStreamTransformEngine& operator|= (const BitStreamInformationEngine& other) noexcept
            {
                *this |= other.storedData->bitStreamTransform;
                return *this;
            }

            /**
             * @brief Bitwise assignment XOR operator.
             *
             * @param [in] other - Constant lvalue reference of the BitStreamTransformEngine class as right operand.
             * @return Lvalue reference of transformed (by operation XOR) BitStreamTransformEngine class.
             *
             * @note If operands have different data length then result data will be the length of the largest among the operands.
             * @note If data handling mode type is DATA_MODE_SAFE_OPERATOR then the length of result data will be saved.
             *
             * @attention The correct result can only be obtained if the data dependent modes are the same.
             */
            BitStreamTransformEngine & operator^= (const BitStreamTransformEngine & /*other*/) noexcept;

            /**
             * @brief Bitwise assignment XOR operator.
             *
             * @param [in] other - Constant lvalue reference of the BitStreamInformationEngine class as right operand.
             * @return Lvalue reference of transformed (by operation XOR) BitStreamTransformEngine class.
             *
             * @note If operands have different data length then result data will be the length of the largest among the operands.
             * @note If data handling mode type is DATA_MODE_SAFE_OPERATOR then the length of result data will be saved.
             *
             * @attention The correct result can only be obtained if the data dependent modes are the same.
             */
            BitStreamTransformEngine& operator^= (const BitStreamInformationEngine& other) noexcept
            {
                *this ^= other.storedData->bitStreamTransform;
                return *this;
            }


            /**
             * @brief Bitwise AND operator.
             *
             * @param [in] left - Constant lvalue reference of the BitStreamTransformEngine class as left operand.
             * @param [in] right - Constant lvalue reference of the BitStreamTransformEngine class as right operand.
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
                BinaryDataEngine result(left.DataEngine());
                if (result == true) {
                    result.BitsTransform() &= right;
                }
                return result;
            }

            /**
             * @brief Bitwise AND operator.
             *
             * @param [in] left - Constant lvalue reference of the BitStreamTransformEngine class as left operand.
             * @param [in] right - Constant lvalue reference of the BitStreamInformationEngine class as right operand.
             * @return New temporary object of transformed (by operation AND) BinaryDataEngine class.
             *
             * @note If operands have different endian and mode then result BinaryDataEngine will have endian and mode of the left operand.
             * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LESSER length.
             *
             * @attention Result BinaryDataEngine class is always processing in data mode type of left operand.
             * @attention The correct result can only be obtained if the data dependent modes are the same.
             */
            friend inline BinaryDataEngine operator& (const BitStreamTransformEngine& left, const BitStreamInformationEngine& right) noexcept
            {
                BinaryDataEngine result(left.DataEngine());
                if (result == true) {
                    result.BitsTransform() &= right;
                }
                return result;
            }

            /**
             * @brief Bitwise OR operator.
             *
             * @param [in] left - Constant lvalue reference of the BitStreamTransformEngine class as left operand.
             * @param [in] right - Constant lvalue reference of the BitStreamTransformEngine class as right operand.
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
                BinaryDataEngine result(left.DataEngine());
                if (result == true) {
                    result.BitsTransform() |= right;
                }
                return result;
            }

            /**
             * @brief Bitwise OR operator.
             *
             * @param [in] left - Constant lvalue reference of the BitStreamTransformEngine class as left operand.
             * @param [in] right - Constant lvalue reference of the BitStreamInformationEngine class as right operand.
             * @return New temporary object of transformed (by operation OR) BinaryDataEngine class.
             *
             * @note If operands have different endian and mode then result BinaryDataEngine will have endian and mode of the left operand.
             * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LONGER length.
             *
             * @attention Result BinaryDataEngine class is always processing in data mode type of left operand.
             * @attention The correct result can only be obtained if the data dependent modes are the same.
             */
            friend inline BinaryDataEngine operator| (const BitStreamTransformEngine& left, const BitStreamInformationEngine& right) noexcept
            {
                BinaryDataEngine result(left.DataEngine());
                if (result == true) {
                    result.BitsTransform() |= right;
                }
                return result;
            }

            /**
             * @brief Bitwise XOR operator.
             *
             * @param [in] left - Constant lvalue reference of the BitStreamTransformEngine class as left operand.
             * @param [in] right - Constant lvalue reference of the BitStreamTransformEngine class as right operand.
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
                BinaryDataEngine result(left.DataEngine());
                if (result == true) {
                    result.BitsTransform() ^= right;
                }
                return result;
            }

            /**
             * @brief Bitwise XOR operator.
             *
             * @param [in] left - Constant lvalue reference of the BitStreamTransformEngine class as left operand.
             * @param [in] right - Constant lvalue reference of the BitStreamInformationEngine class as right operand.
             * @return New temporary object of transformed (by operation XOR) BinaryDataEngine class.
             *
             * @note If operands have different endian and mode then result BinaryDataEngine will have endian and mode of the left operand.
             * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LONGER length.
             *
             * @attention Result BinaryDataEngine class is always processing in data mode type of left operand.
             * @attention The correct result can only be obtained if the data dependent modes are the same.
             */
            friend inline BinaryDataEngine operator^ (const BitStreamTransformEngine& left, const BitStreamInformationEngine& right) noexcept
            {
                BinaryDataEngine result(left.DataEngine());
                if (result == true) {
                    result.BitsTransform() ^= right;
                }
                return result;
            }

            /**
             * @brief Bitwise left shift operator that performs direct left bit shift by a specified bit offset.
             *
             * @param [in] engine - Constant lvalue reference of the BitStreamTransformEngine class as left operand.
             * @param [in] shift - Bit offset for direct left bit shift as right operand.
             * @return New temporary object of transformed BinaryDataEngine class.
             */
            friend inline BinaryDataEngine operator<< (const BitStreamTransformEngine& engine, const std::size_t shift) noexcept
            {
                BinaryDataEngine result(engine.DataEngine());
                result.BitsTransform().ShiftLeft(shift, false);
                return result;
            }

            /**
             * @brief Bitwise right shift operator that performs direct right bit shift by a specified bit offset.
             *
             * @param [in] engine - Constant lvalue reference of the BitStreamTransformEngine class as left operand.
             * @param [in] shift - Bit offset for direct right bit shift as right operand.
             * @return New temporary object of transformed BinaryDataEngine class.
             */
            friend inline BinaryDataEngine operator>> (const BitStreamTransformEngine& engine, const std::size_t shift) noexcept
            {
                BinaryDataEngine result(engine.DataEngine());
                result.BitsTransform().ShiftRight(shift, false);
                return result;
            }

            /**
             * @brief Bitwise complement operator that inverts each bit in internal binary data.
             *
             * @param [in] engine - Constant lvalue reference of the BitStreamTransformEngine class.
             * @return New temporary object of transformed (by operation NOT) BinaryDataEngine class.
             */
            friend inline BinaryDataEngine operator~ (const BitStreamTransformEngine& engine) noexcept
            {
                BinaryDataEngine result(engine.DataEngine());
                result.BitsTransform().InvertBlock();
                return result;
            }
        };


        /**
         * @class ByteStreamInformationEngine   BinaryDataEngine.hpp   "include/framework/BinaryDataEngine.hpp"
         *
         * @brief Class that operates on a sequence of bytes and offers an interface for working with them.
         *
         * @attention This class MUST BE initialized in all constructors of owner class.
         */
        class ByteStreamInformationEngine
        {
            friend class IntegerValue;
            friend class BinaryDataEngine;
            friend class BitStreamTransformEngine;
            friend class BitStreamInformationEngine;
            friend class ByteStreamEngine;

            template <typename BinaryDataType>
            friend class BinaryStructuredDataEngineBase;

        private:
            /**
             * @brief Constant pointer to the constant BinaryDataEngine owner class.
             */
            const BinaryDataEngine * const storedData;

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
            ByteStreamInformationEngine(void) = delete;
            ByteStreamInformationEngine (ByteStreamInformationEngine &&) = delete;
            ByteStreamInformationEngine (const ByteStreamInformationEngine &) = delete;
            ByteStreamInformationEngine & operator= (ByteStreamInformationEngine &&) = delete;
            ByteStreamInformationEngine & operator= (const ByteStreamInformationEngine &) = delete;

            /**
             * @brief Constructor of nested ByteStreamInformationEngine class.
             *
             * @param [in] owner - Constant pointer to the const BinaryDataEngine owner class.
             */
            explicit ByteStreamInformationEngine (const BinaryDataEngine* const owner) noexcept
                : storedData(owner)
            { }

            /**
             * @brief Default destructor of nested ByteStreamInformationEngine class.
             */
            ~ByteStreamInformationEngine(void) noexcept = default;

            /**
             * @brief Method that returns the length of stored data in bytes.
             *
             * @return Length of byte sequence of stored data.
             */
            inline std::size_t Length(void) const noexcept { return storedData->length; }

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
             * @param [in] first - First index of byte in binary sequence from which sequent bytes will be checked. Default: 0.
             * @param [in] last - Last index of byte in binary sequence to which (inclusive) bytes will be checked. Default: npos.
             * @param [in] value - Value of the pattern byte for check. Default: 0xFF.
             * @return TRUE - if all bytes in block of stored data have a specified value, otherwise - FALSE.
             *
             * @warning Method always returns 'false' if the index is out-of-range.
             */
            bool All (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos, std::byte /*value*/ = HighByte) const noexcept;

            /**
             * @brief Method that returns byte sequence characteristic when any of the bytes have a specified value in block of stored data.
             *
             * @param [in] first - First index of byte in binary sequence from which sequent bytes will be checked. Default: 0.
             * @param [in] last - Last index of byte in binary sequence to which (inclusive) bytes will be checked. Default: npos.
             * @param [in] value - Value of the pattern byte for check. Default: 0xFF.
             * @return TRUE - if any of the bytes in block of stored data have a specified value, otherwise - FALSE.
             *
             * @warning Method always returns 'false' if the index is out-of-range.
             */
            bool Any (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos, std::byte /*byte*/ = HighByte) const noexcept;

            /**
             * @brief Method that returns byte sequence characteristic when none of the bytes have a specified value in block of stored data.
             *
             * @param [in] first - First index of byte in binary sequence from which sequent bytes will be checked. Default: 0.
             * @param [in] last - Last index of byte in binary sequence to which (inclusive) bytes will be checked. Default: npos.
             * @param [in] value - Value of the pattern byte for check. Default: 0x00.
             * @return TRUE - if all of the bytes in block of stored data have not a specified value, otherwise - FALSE.
             *
             * @warning Method always returns 'false' if the index is out-of-range.
             */
            bool None (std::size_t /*first*/ = 0, std::size_t /*last*/ = npos, std::byte /*byte*/ = LowByte) const noexcept;

            /**
             * @brief Operator that returns the value of byte under the specified index.
             *
             * @param [in] index - Index of byte in stored binary data.
             * @return Value of the selected byte.
             */
            inline std::optional<std::byte> operator[] (const std::size_t index) const noexcept
            {
                if (index >= Length()) { return std::nullopt; }
                return storedData->data[GetBytePosition(index)];
            }

            /**
             * @brief Method that returns a constant pointer to the value of byte under the specified endian-oriented byte index.
             *
             * @param [in] index - Index of byte in stored binary data.
             * @return Return a constant pointer to the value of byte under the specified index or nullptr if index is out-of-range.
             *
             * @note This method considers the endian type in which binary stored data are presented.
             */
            [[nodiscard]]
            const std::byte * GetAt (std::size_t /*index*/) const noexcept;
        };


        /**
         * @class ByteStreamTransformEngine   BinaryDataEngine.hpp   "include/framework/BinaryDataEngine.hpp"
         *
         * @brief Class that operates on a sequence of bytes and offers an interface for working with them.
         *
         * @attention This class MUST BE initialized in all constructors of owner class.
         */
        class ByteStreamTransformEngine
        {
            friend class IntegerValue;
            friend class BinaryDataEngine;
            friend class BitStreamTransformEngine;
            friend class BitStreamInformationEngine;
            friend class ByteStreamInformationEngine;

            template <typename BinaryDataType>
            friend class BinaryStructuredDataEngineBase;

        private:
            /**
             * @brief Constant pointer to the BinaryDataEngine owner class.
             */
            BinaryDataEngine * const storedData;

        public:
            ByteStreamTransformEngine(void) = delete;
            ByteStreamTransformEngine (ByteStreamTransformEngine &&) = delete;
            ByteStreamTransformEngine (const ByteStreamTransformEngine &) = delete;
            ByteStreamTransformEngine & operator= (ByteStreamTransformEngine &&) = delete;
            ByteStreamTransformEngine & operator= (const ByteStreamTransformEngine &) = delete;

            /**
             * @brief Constructor of nested ByteStreamTransformEngine class.
             *
             * @param [in] owner - Lvalue reference of BinaryDataEngine owner class.
             */
            explicit ByteStreamTransformEngine (BinaryDataEngine * const owner) noexcept
                : storedData(owner)
            { }

            /**
             * @brief Default destructor of nested ByteStreamTransformEngine class.
             */
            ~ByteStreamTransformEngine(void) noexcept = default;

            /**
             * @brief Method that returns the length of stored data in bytes.
             *
             * @return Length of byte sequence of stored data.
             */
            inline std::size_t Length(void) const noexcept { return storedData->length; }

            /**
             * @brief Method that sets the byte under the specified index to new value.
             *
             * @param [in] index - Index of byte in byte sequence.
             * @param [in] fillByte - New value of selected byte. Default: 0xFF.
             * @return Lvalue reference of modified ByteStreamTransformEngine class.
             */
            ByteStreamTransformEngine & Set (std::size_t /*index*/, std::byte /*fillByte*/ = HighByte) noexcept;

            /**
             * @brief Method that performs direct left byte shift by a specified byte offset.
             *
             * @param [in] shift - Byte offset for direct left byte shift.
             * @param [in] fillByte - Value of the fill byte after the left shift. Default: 0x00.
             * @return Lvalue reference of ByteStreamTransformEngine class.
             */
            ByteStreamTransformEngine & ShiftLeft (std::size_t /*shift*/, std::byte /*fillByte*/ = LowByte) noexcept;

            /**
             * @brief Method that performs direct right byte shift by a specified byte offset.
             *
             * @param [in] shift - Byte offset for direct right byte shift.
             * @param [in] fillByte - Value of the fill byte after the right shift. Default: 0x00.
             * @return Lvalue reference of ByteStreamTransformEngine class.
             */
            ByteStreamTransformEngine & ShiftRight (std::size_t /*shift*/, std::byte /*fillByte*/ = LowByte) noexcept;

            /**
             * @brief Method that performs round left byte shift by a specified byte offset.
             *
             * @param [in] shift - Byte offset for round left byte shift.
             * @return Lvalue reference of ByteStreamTransformEngine class.
             */
            ByteStreamTransformEngine & RoundShiftLeft (std::size_t /*shift*/) noexcept;

            /**
             * @brief Method that performs round right byte shift by a specified byte offset.
             *
             * @param [in] shift - Byte offset for round right byte shift.
             * @return Lvalue reference of ByteStreamTransformEngine class.
             */
            ByteStreamTransformEngine & RoundShiftRight (std::size_t /*shift*/) noexcept;

            /**
             * @brief Operator that returns the value of byte under the specified index.
             *
             * @param [in] index - Index of byte in stored binary data.
             * @return Value of the selected byte.
             *
             * @note This method considers the endian type in which binary stored data are presented.
             */
            inline std::optional<std::byte*> operator[] (const std::size_t index) noexcept
            {
                if (index >= Length()) { return std::nullopt; }
                return &storedData->data[storedData->byteStreamInformation.GetBytePosition(index)];
            }

            /**
             * @brief Method that returns a pointer to the value of byte under the specified endian-oriented byte index.
             *
             * @param [in] index - Index of byte in stored binary data.
             * @return Return a pointer to the value of byte under the specified index or nullptr if index is out-of-range.
             *
             * @note This method considers the endian type in which binary stored data are presented.
             */
            [[nodiscard]]
            std::byte * GetAt (std::size_t /*index*/) noexcept;
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
        DATA_ENDIAN_TYPE dataEndianType = system_endian;
        /**
         * @brief Information engine for working with sequence of bits.
         */
        BitStreamInformationEngine bitStreamInformation;
        /**
         * @brief Transformation engine for working with sequence of bits.
         */
        BitStreamTransformEngine bitStreamTransform;
        /**
         * @brief Information engine for working with sequence of bytes.
         */
        ByteStreamInformationEngine byteStreamInformation;
        /**
         * @brief Transformation engine for working with sequence of bytes.
         */
        ByteStreamTransformEngine byteStreamTransform;


    public:
        /**
         * @brief Constructor of BinaryDataEngine class.
         *
         * @param [in] mode - Type of the data handling mode. Default: DATA_DEFAULT_MODE.
         * @param [in] endian - Endian of stored data. Default: Local System Endian Type (DATA_SYSTEM_ENDIAN).
         */
        explicit BinaryDataEngine (const uint8_t mode = DATA_MODE_DEFAULT, const DATA_ENDIAN_TYPE endian = DATA_SYSTEM_ENDIAN) noexcept
            : dataModeType(mode), dataEndianType(endian),
              bitStreamInformation(this), bitStreamTransform(this),
              byteStreamInformation(this), byteStreamTransform(this)
        {
            if (dataEndianType == DATA_SYSTEM_ENDIAN) {
                dataEndianType = system_endian;
            }
        }

        /**
         * @brief Destructor of BinaryDataEngine class.
         */
        ~BinaryDataEngine(void) noexcept { Reset(); }

        /**
         * @brief Copy assignment constructor of BinaryDataEngine class.
         *
         * @param [in] other - Constant lvalue reference of copied BinaryDataEngine class.
         *
         * @note After data assignment the data handling mode is changed to DATA_MODE_ALLOCATION.
         *
         * @attention Need to check existence of data after use this constructor.
         */
        BinaryDataEngine (const BinaryDataEngine & /*other*/) noexcept;

        /**
         * @brief Move assignment constructor of BinaryDataEngine class.
         *
         * @param [in] other - Rvalue reference of moved BinaryDataEngine class.
         */
        BinaryDataEngine (BinaryDataEngine && /*other*/) noexcept;

        /**
         * @brief Constructor that allocates specified amount of bytes.
         *
         * @param [in] size - Number of bytes for allocate.
         * @param [in] mode - Type of the data handling mode. Default: DATA_DEFAULT_MODE.
         * @param [in] endian - Endian of stored data. Default: Local System Endian Type (DATA_SYSTEM_ENDIAN).
         *
         * @note After data assignment the data handling mode is changed to DATA_MODE_ALLOCATION.
         *
         * @attention Need to check existence of data after use this constructor.
         */
        explicit BinaryDataEngine (std::size_t /*size*/, uint8_t /*mode*/ = DATA_MODE_DEFAULT, DATA_ENDIAN_TYPE /*endian*/ = DATA_SYSTEM_ENDIAN) noexcept;

        /**
         * @brief Constructor that accepts a pointer to allocated (or static) binary data.
         *
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
                          DATA_ENDIAN_TYPE /*endian*/   = DATA_SYSTEM_ENDIAN,
                          uint8_t          /*mode*/     = DATA_MODE_DEFAULT,
                          bool             /*destruct*/ = false) noexcept;

        /**
         * @brief Constructor that accepts a pointer to const allocated (or static) binary data.
         *
         * @param [in] memory - Pointer to const allocated (or static) data.
         * @param [in] size - Number of bytes in data.
         * @param [in] endian - Endian of inputted data. Default: Local System Endian Type (DATA_SYSTEM_ENDIAN).
         * @param [in] mode - Type of the data handling mode. Default: DATA_DEFAULT_MODE.
         *
         * @attention Need to check existence of data after use this constructor.
         */
        BinaryDataEngine (const std::byte * /*memory*/,
                          std::size_t       /*size*/,
                          DATA_ENDIAN_TYPE  /*endian*/  = DATA_SYSTEM_ENDIAN,
                          uint8_t           /*mode*/    = DATA_MODE_DEFAULT) noexcept;

        /**
         * @brief Copy assignment operator of BinaryDataEngine class.
         *
         * @param [in] other - Constant lvalue reference of copied BinaryDataEngine class.
         * @return Lvalue reference of copied BinaryDataEngine class.
         *
         * @note After data assignment the data handling mode is changed to DATA_MODE_ALLOCATION.
         *
         * @attention Need to check existence of data after use this operator.
         */
        BinaryDataEngine & operator= (const BinaryDataEngine & /*other*/) noexcept;

        /**
         * @brief Move assignment operator of BinaryDataEngine class.
         *
         * @param [in] other - Rvalue reference of moved BinaryDataEngine class.
         * @return Lvalue reference of moved BinaryDataEngine class.
         */
        BinaryDataEngine & operator= (BinaryDataEngine && /*other*/) noexcept;

        /**
         * @brief Method that assigns data to BinaryDataEngine.
         *
         * @tparam [in] Type - Typename of assigned data.
         * @param [in] memory - Pointer to the constant data of selected type.
         * @param [in] count - The number of elements in the inputted data of selected type.
         * @return TRUE - if data assignment is successful, otherwise - FALSE.
         *
         * @note After data assignment the data handling mode is changed to DATA_MODE_ALLOCATION.
         * @note Input type MUST be a POD type.
         */
        template <typename Type>
        [[nodiscard]]
        bool AssignData (const Type* const memory, const std::size_t count) noexcept
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
            else {
                memcpy(data.get(), memory, length);
            }
            return true;
        }

        /**
         * @brief Method that assigns data to BinaryDataEngine.
         *
         * @tparam [in] Type - Typename of iterator of assigned data.
         * @param [in] begin - Iterator to the first element of const data of selected type.
         * @param [in] end - Iterator to the element following the last one of const data of selected type.
         * @return TRUE - if data assignment is successful, otherwise - FALSE.
         *
         * @note After data assignment the data handling mode is changed to DATA_MODE_ALLOCATION.
         * @note Input type under iterator MUST be a POD type.
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
         *
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
         *
         * @return Lvalue reference of the BitStreamTransformEngine class.
         */
        inline BitStreamTransformEngine& BitsTransform(void) noexcept { return bitStreamTransform; }

        /**
         * @brief Method that returns const lvalue reference of the nested BitStreamInformationEngine class for working with bits.
         *
         * @return Constant lvalue reference of the BitStreamInformationEngine class.
         */
        inline const BitStreamInformationEngine& BitsInformation(void) const noexcept { return bitStreamInformation; }

        /**
         * @brief Method that returns lvalue reference of the nested ByteStreamEngine class for working with bytes.
         *
         * @return Lvalue reference of the ByteStreamTransformEngine class.
         */
        inline ByteStreamTransformEngine& BytesTransform(void) noexcept { return byteStreamTransform; }

        /**
         * @brief Method that returns lvalue reference of the nested ByteStreamInformationEngine class for working with bytes.
         *
         * @return Constant lvalue reference of the ByteStreamInformationEngine class.
         */
        inline const ByteStreamInformationEngine& BytesInformation(void) const noexcept { return byteStreamInformation; }

        /**
         * @brief Method that returns the size of stored data.
         *
         * @return Size of stored data in bytes.
         */
        inline std::size_t Size(void) const noexcept { return length; }

        /**
         * @brief Method that returns the pointer to the const internal data.
         *
         * @return Pointer to the const internal data.
         *
         * @note This method does not consider the type of endian in which stored data are presented.
         */
        inline const std::byte* Data(void) const noexcept { return data.get(); }

        /**
         * @brief Method that returns the state of stored binary data in BinaryDataEngine class.
         *
         * @return TRUE - if stored data is empty, otherwise - FALSE.
         */
        inline bool IsEmpty(void) const noexcept { return length == 0; }

        /**
         * @brief Method that returns the initialize state of stored data in BinaryDataEngine class.
         *
         * @return TRUE - if stored data is initialized, otherwise - FALSE.
         */
        inline bool IsInitialized(void) const noexcept { return data != nullptr; }

        /**
         * @brief Method that returns the data handling mode type of stored binary data in BinaryDataEngine class.
         *
         * @return The set of enabled modes of DATA_HANDLING_MODE enum.
         */
        inline uint8_t DataModeType(void) const noexcept { return dataModeType; }

        /**
         * @brief Method that returns the endian type of stored data in BinaryDataEngine class.
         *
         * @return DATA_LITTLE_ENDIAN(0x02) - if on the system little endian, otherwise - DATA_BIG_ENDIAN(0x01).
         */
        inline DATA_ENDIAN_TYPE DataEndianType(void) const noexcept { return dataEndianType; }

        /**
         * @brief Method that changes handling mode type of stored data in BinaryDataEngine class.
         *
         * @param [in] mode - New data handling mode type.
         *
         * @note Changed only the method of processing data. Data representation does not changes.
         * @note After setting a new mode then the opposite mode will be automatically turned off.
         */
        void SetDataModeType (uint8_t /*mode*/) const noexcept;

        /**
         * @brief Method that changes endian type of stored data in BinaryDataEngine class.
         *
         * @param [in] endian - New data endian type.
         * @param [in] convert - Flag indicating whether to change the presentation of the stored data or not. Default: true.
         */
        void SetDataEndianType (DATA_ENDIAN_TYPE /*endian*/, bool /*convert*/ = true) noexcept;

        /**
         * @brief Method that returns the data dependent mode type.
         *
         * @return TRUE - if the data handling mode consist of the DATA_MODE_DEPENDENT flag, otherwise - FALSE.
         */
        inline bool IsDependentDataMode(void) const noexcept { return ((dataModeType & DATA_MODE_DEPENDENT) != 0U); }

        /**
         * @brief Method that returns the binary operators behavior type when transformed the stored data.
         *
         * @return TRUE - if the data handling mode consist of the DATA_MODE_SAFE_OPERATOR flag, otherwise - FALSE.
         */
        inline bool IsSafeOperatorDataMode(void) const noexcept { return ((dataModeType & DATA_MODE_SAFE_OPERATOR) != 0U); }

        /**
         * @brief Method that returns the data allocation mode type.
         *
         * @return TRUE - if the data handling mode consist of the DATA_MODE_ALLOCATION flag, otherwise - FALSE.
         */
        inline bool IsAllocationDataMode(void) const noexcept { return ((dataModeType & DATA_MODE_ALLOCATION) != 0U); }

        /**
         * @brief Method that returns the binary operators behavior type when transformed the stored data.
         *
         * @return TRUE - if the data handling mode consist of the DATA_MODE_OPERATOR_ALIGN_LOW_ORDER flag, otherwise - FALSE.
         */
        inline bool IsOperatorAlignLowOrderDataMode(void) const noexcept { return ((dataModeType & DATA_MODE_OPERATOR_ALIGN_LOW_ORDER) != 0U); }

        /**
         * @brief Method that returns a pointer to an element by selected index.
         *
         * @param [in] index - Index of byte in byte sequence of stored data.
         * @return Return a pointer to an element by selected index or nullptr in an error occurred.
         *
         * @note This method DOES NOT consider the endian type in which stored data are presented.
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
         *
         * @return String that represent internal binary data.
         */
        std::string ToHexString(void) const noexcept;

        /**
         * @brief Method returns internal data in standard uint8_t type.
         *
         * @return Internal data in uint8_t representation.
         */
        std::optional<uint8_t> To8Bit(void) const noexcept;

        /**
         * @brief Method returns internal data in standard uint16_t type.
         *
         * @return Internal data in uint16_t representation.
         */
        std::optional<uint16_t> To16Bit(void) const noexcept;

        /**
         * @brief Method returns internal data in standard uint32_t type.
         *
         * @return Internal data in uint32_t representation.
         */
        std::optional<uint32_t> To32Bit(void) const noexcept;

        /**
         * @brief Method returns internal data in standard uint64_t type.
         *
         * @return Internal data in uint64_t representation.
         */
        std::optional<uint64_t> To64Bit(void) const noexcept;

        /**
         * @brief Operator that returns the internal state of BinaryDataEngine class.
         *
         * @return TRUE - if BinaryDataEngine class is not empty, otherwise - FALSE.
         */
        inline operator bool(void) const noexcept { return (data != nullptr && length != 0); }

        /**
         * @brief Operator that returns a byte of data under selected index.
         *
         * @param [in] index - Index of byte in byte sequence of stored data.
         * @return Return a byte of data under selected index.
         *
         * @note This method does not consider the type of endian in which data are presented.
         */
        std::optional<std::byte> operator[] (std::size_t /*index*/) const noexcept;

        /**
         * @brief Bitwise assignment AND operator.
         *
         * @param [in] other - Constant lvalue reference of the BinaryDataEngine class as right operand.
         * @return Lvalue reference of transformed (by operation AND) BinaryDataEngine class.
         *
         * @note If operands have different data length then result data will be the length of the lesser among the operands.
         * @note If data handling mode type is DATA_MODE_SAFE_OPERATOR then the size of result data will be saved.
         *
         * @attention The correct result can only be obtained if the data dependent modes are the same.
         */
        BinaryDataEngine & operator&= (const BinaryDataEngine & /*other*/) noexcept;

        /**
         * @brief Bitwise assignment OR operator.
         *
         * @param [in] other - Constant lvalue reference of the BinaryDataEngine class as right operand.
         * @return Lvalue reference of transformed (by operation OR) BinaryDataEngine class.
         *
         * @note If operands have different data length then result data will be the length of the largest among the operands.
         * @note If data handling mode type is DATA_MODE_SAFE_OPERATOR then the size of result data will be saved.
         *
         * @attention The correct result can only be obtained if the data dependent modes are the same.
         */
        BinaryDataEngine & operator|= (const BinaryDataEngine & /*other*/) noexcept;

        /**
         * @brief Bitwise assignment XOR operator.
         *
         * @param [in] other - Constant lvalue reference of the BinaryDataEngine class as right operand.
         * @return Lvalue reference of transformed (by operation XOR) BinaryDataEngine class.
         *
         * @note If operands have different data length then result data will be the length of the largest among the operands.
         * @note If data handling mode type is DATA_MODE_SAFE_OPERATOR then the length of result data will be saved.
         *
         * @attention The correct result can only be obtained if the data dependent modes are the same.
         */
        BinaryDataEngine & operator^= (const BinaryDataEngine & /*other*/) noexcept;

    };


    /**
     * @brief Bitwise AND operator.
     *
     * @param [in] left - Constant lvalue reference of the BinaryDataEngine class as left operand.
     * @param [in] right - Constant lvalue reference of the BinaryDataEngine class as right operand.
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
        if (result == true)
        {
            if (result.DataEndianType() == right.DataEndianType() && result.Size() == right.Size())
            {
                const std::size_t length = result.Size();
                for (std::size_t idx = 0; idx < length; )
                {
                    if (idx + sizeof(uint32_t) < length)
                    {
                        (*reinterpret_cast<uint32_t*>(result.GetAt(idx))) &= (*reinterpret_cast<const uint32_t*>(right.GetAt(idx)));
                        idx += sizeof(uint32_t);
                    }
                    else
                    {
                        (*result.GetAt(idx)) &= right[idx].value();
                        ++idx;
                    }
                }
            }
            else { result.BitsTransform() &= right.BitsInformation(); }
        }
        return result;
    }

    /**
     * @brief Bitwise OR operator.
     *
     * @param [in] left - Constant lvalue reference of the BinaryDataEngine class as left operand.
     * @param [in] right - Constant lvalue reference of the BinaryDataEngine class as right operand.
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
        if (result == true)
        {
            if (result.DataEndianType() == right.DataEndianType() && result.Size() == right.Size())
            {
                const std::size_t length = result.Size();
                for (std::size_t idx = 0; idx < length; )
                {
                    if (idx + sizeof(uint32_t) < length)
                    {
                        (*reinterpret_cast<uint32_t*>(result.GetAt(idx))) |= (*reinterpret_cast<const uint32_t*>(right.GetAt(idx)));
                        idx += sizeof(uint32_t);
                    }
                    else
                    {
                        (*result.GetAt(idx)) |= right[idx].value();
                        ++idx;
                    }
                }
            }
            else { result.BitsTransform() |= right.BitsInformation(); }
        }
        return result;
    }

    /**
     * @brief Bitwise XOR operator.
     *
     * @param [in] left - Constant lvalue reference of the BinaryDataEngine class as left operand.
     * @param [in] right - Constant lvalue reference of the BinaryDataEngine class as right operand.
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
        if (result == true)
        {
            if (result.DataEndianType() == right.DataEndianType() && result.Size() == right.Size())
            {
                const std::size_t length = result.Size();
                for (std::size_t idx = 0; idx < length; )
                {
                    if (idx + sizeof(uint32_t) < length)
                    {
                        (*reinterpret_cast<uint32_t*>(result.GetAt(idx))) ^= (*reinterpret_cast<const uint32_t*>(right.GetAt(idx)));
                        idx += sizeof(uint32_t);
                    }
                    else
                    {
                        (*result.GetAt(idx)) ^= right[idx].value();
                        ++idx;
                    }
                }
            }
            else { result.BitsTransform() ^= right.BitsInformation(); }
        }
        return result;
    }

    /**
     * @brief Literal operator that converts number in string representation to 8-bit integer in little endian form.
     *
     * @param [in] value - Number in string representation.
     * @param [in] size - Length of the input string.
     * @return BinaryDataEngine type that contains 8-bit integer in little endian form.
     */
    std::optional<BinaryDataEngine> operator "" _u8_le (const char * /*value*/, std::size_t /*size*/) noexcept;

    /**
     * @brief Literal operator that converts number in string representation to 8-bit integer in big endian form.
     *
     * @param [in] value - Number in string representation.
     * @param [in] size - Length of the input string.
     * @return BinaryDataEngine type that contains 8-bit integer in big endian form.
     */
    std::optional<BinaryDataEngine> operator "" _u8_be (const char * /*value*/, std::size_t /*size*/) noexcept;

    /**
     * @brief Literal operator that converts number in string representation to 8-bit integer in reverse big endian form.
     *
     * @param [in] value - Number in string representation.
     * @param [in] size - Length of the input string.
     * @return BinaryDataEngine type that contains 8-bit integer in reverse big endian form.
     */
    std::optional<BinaryDataEngine> operator "" _u8_rbe (const char * /*value*/, std::size_t /*size*/) noexcept;

    /**
     * @brief Literal operator that converts number in string representation to 16-bit integer in little endian form.
     *
     * @param [in] value - Number in string representation.
     * @param [in] size - Length of the input string.
     * @return BinaryDataEngine type that contains 16-bit integer in little endian form.
     */
    std::optional<BinaryDataEngine> operator "" _u16_le (const char * /*value*/, std::size_t /*size*/) noexcept;

    /**
     * @brief Literal operator that converts number in string representation to 16-bit integer in big endian form.
     *
     * @param [in] value - Number in string representation.
     * @param [in] size - Length of the input string.
     * @return BinaryDataEngine type that contains 16-bit integer in big endian form.
     */
    std::optional<BinaryDataEngine> operator "" _u16_be (const char * /*value*/, std::size_t /*size*/) noexcept;

    /**
     * @brief Literal operator that converts number in string representation to 16-bit integer in reverse big endian form.
     *
     * @param [in] value - Number in string representation.
     * @param [in] size - Length of the input string.
     * @return BinaryDataEngine type that contains 16-bit integer in reverse big endian form.
     */
    std::optional<BinaryDataEngine> operator "" _u16_rbe (const char * /*value*/, std::size_t /*size*/) noexcept;

    /**
     * @brief Literal operator that converts number in string representation to 32-bit integer in little endian form.
     *
     * @param [in] value - Number in string representation.
     * @param [in] size - Length of the input string.
     * @return BinaryDataEngine type that contains 32-bit integer in little endian form.
     */
    std::optional<BinaryDataEngine> operator "" _u32_le (const char * /*value*/, std::size_t /*size*/) noexcept;

    /**
     * @brief Literal operator that converts number in string representation to 32-bit integer in big endian form.
     *
     * @param [in] value - Number in string representation.
     * @param [in] size - Length of the input string.
     * @return BinaryDataEngine type that contains 32-bit integer in big endian form.
     */
    std::optional<BinaryDataEngine> operator "" _u32_be (const char * /*value*/, std::size_t /*size*/) noexcept;

    /**
     * @brief Literal operator that converts number in string representation to 32-bit integer in reverse big endian form.
     *
     * @param [in] value - Number in string representation.
     * @param [in] size - Length of the input string.
     * @return BinaryDataEngine type that contains 32-bit integer in reverse big endian form.
     */
    std::optional<BinaryDataEngine> operator "" _u32_rbe (const char * /*value*/, std::size_t /*size*/) noexcept;

    /**
     * @brief Literal operator that converts number in string representation to 64-bit integer in little endian form.
     *
     * @param [in] value - Number in string representation.
     * @param [in] size - Length of the input string.
     * @return BinaryDataEngine type that contains 64-bit integer in little endian form.
     */
    std::optional<BinaryDataEngine> operator "" _u64_le (const char * /*value*/, std::size_t /*size*/) noexcept;

    /**
     * @brief Literal operator that converts number in string representation to 64-bit integer in big endian form.
     *
     * @param [in] value - Number in string representation.
     * @param [in] size - Length of the input string.
     * @return BinaryDataEngine type that contains 64-bit integer in big endian form.
     */
    std::optional<BinaryDataEngine> operator "" _u64_be (const char * /*value*/, std::size_t /*size*/) noexcept;

    /**
     * @brief Literal operator that converts number in string representation to 64-bit integer in reverse big endian form.
     *
     * @param [in] value - Number in string representation.
     * @param [in] size - Length of the input string.
     * @return BinaryDataEngine type that contains 64-bit integer in reverse big endian form.
     */
    std::optional<BinaryDataEngine> operator "" _u64_rbe (const char * /*value*/, std::size_t /*size*/) noexcept;

}  // namespace types.

#endif  // PROTOCOL_ANALYZER_BINARY_DATA_ENGINE_HPP
