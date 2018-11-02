// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#ifndef PROTOCOL_ANALYZER_BINARY_STRUCTURED_DATA_ENGINE_HPP
#define PROTOCOL_ANALYZER_BINARY_STRUCTURED_DATA_ENGINE_HPP

#include <optional>  // std::optional.

#include "BinaryDataEngine.hpp"  // types::BinaryDataEngine.


namespace analyzer::framework::common::types
{
/**
 * @def STRUCTURED_DATA_HANDLING_MODE;
 * @brief Marco that include a default settings for structured data handling mode in BinaryStructuredDataEngine class.
 *
 * @note This data handling mode does not changed in BinaryStructuredDataEngine class.
 */
#define STRUCTURED_DATA_HANDLING_MODE   (DATA_MODE_INDEPENDENT | DATA_MODE_SAFE_OPERATOR | DATA_MODE_ALLOCATION | DATA_MODE_OPERATOR_ALIGN_LOW_ORDER)


    /**
     * @class BinaryStructuredDataEngine   BinaryStructuredDataEngine.hpp   "include/framework/BinaryStructuredDataEngine.hpp"
     * @brief Main class of analyzer framework that contains binary structured data and gives an interface to work with it.
     *
     * @note This class is cross-platform.
     */
    class BinaryStructuredDataEngine
    {
    private:
        /**
         * @var BinaryDataEngine data;
         * @brief Variable that stores binary structured data.
         *
         * @note This variable is always represents in STRUCTURED_DATA_HANDLING_MODE mode.
         */
        BinaryDataEngine data;
        /**
         * @var uint16_t fieldsCount;
         * @brief Count of fields in stored structured data.
         */
        uint16_t fieldsCount = 0;
        /**
         * @var std::unique_ptr<uint16_t[]> dataPattern;
         * @brief Array that contains the pattern of stored structured data in bits.
         */
        std::unique_ptr<uint16_t[]> dataPattern = nullptr;
        /**
         * @var DATA_ENDIAN_TYPE dataEndianType;
         * @brief Endian type of stored structured data.
         */
        DATA_ENDIAN_TYPE dataEndianType = BinaryDataEngine::system_endian;


        /**
         * @fn template <uint8_t Mode>
         * std::size_t BinaryStructuredDataEngine::GetBitOffset (const uint16_t, const uint16_t) const noexcept;
         * @brief Method that returns bit offset in selected field bit index of structured data.
         * @tparam [in] Mode - Type of input data handling mode. Default: DATA_MODE_INDEPENDENT.
         * @param [in] fieldIndex - Index of field in structured data.
         * @param [in] bitIndex - Bit index in selected field of structured data.
         * @return Bit offset under selected field bit index of structured data.
         *
         * @note The resulting bit offset is obtained in DATA_MODE_INDEPENDENT handling mode.
         * @note Method returns index 'BinaryDataEngine::npos' if selected index is out-of-range.
         * @note Return value is marked with the "nodiscard" attribute.
         */
        template <uint8_t Mode = DATA_MODE_INDEPENDENT>
        [[nodiscard]]
        std::size_t GetBitOffset (const uint16_t fieldIndex, const uint16_t bitIndex) const noexcept
        {
            if (fieldIndex < fieldsCount && bitIndex < dataPattern[fieldIndex] * 8)
            {
                const std::size_t offset = static_cast<std::size_t>(std::accumulate(dataPattern.get(), dataPattern.get() + fieldIndex, 0)) * 8;
                if constexpr (Mode == DATA_MODE_INDEPENDENT) { return offset + bitIndex; }

                if (dataEndianType == DATA_BIG_ENDIAN) {
                    return offset + dataPattern[fieldIndex] * 8 - bitIndex - 1;
                }
                // If data endian type is DATA_LITTLE_ENDIAN.
                return offset + (bitIndex >> 3) * 8 - bitIndex % 8 + 7;
            }
            return BinaryDataEngine::npos;
        }

    public:

        /**
         * @fn BinaryStructuredDataEngine::BinaryStructuredDataEngine (const DATA_ENDIAN_TYPE) noexcept;
         * @brief Constructor of BinaryStructuredDataEngine class.
         * @param [in] endian - Endian of stored structured data. Default: Local System Type (DATA_SYSTEM_ENDIAN).
         */
        BinaryStructuredDataEngine (const DATA_ENDIAN_TYPE endian = BinaryDataEngine::system_endian) noexcept
                : data(BinaryDataEngine(STRUCTURED_DATA_HANDLING_MODE, DATA_BIG_ENDIAN)), dataEndianType(endian)
        { }

        /**
         * @fn BinaryStructuredDataEngine::~BinaryStructuredDataEngine() noexcept;
         * @brief Destructor of BinaryStructuredDataEngine class.
         */
        ~BinaryStructuredDataEngine(void) noexcept { Reset(); }

        /**
         * @fn BinaryStructuredDataEngine::BinaryStructuredDataEngine (BinaryDataEngine &, const uint16_t *, uint16_t, DATA_ENDIAN_TYPE) noexcept;
         * @brief Constructor of BinaryStructuredDataEngine class with prepared structured data.
         * @param [in] input - Lvalue reference of moved BinaryDataEngine class.
         * @param [in] pattern - Array that contains the pattern of inputted structure data in bytes.
         * @param [in] size - Size of the pattern array.
         * @param [in] endian - Endian of stored structured data. Default: Local System Type (DATA_SYSTEM_ENDIAN).
         */
        BinaryStructuredDataEngine (BinaryDataEngine & /*input*/, const uint16_t * /*pattern*/, uint16_t /*size*/, DATA_ENDIAN_TYPE endian = BinaryDataEngine::system_endian) noexcept;

        /**
         * @fn BinaryStructuredDataEngine::BinaryStructuredDataEngine (const BinaryStructuredDataEngine &) noexcept;
         * @brief Copy assignment constructor of BinaryStructuredDataEngine class.
         * @param [in] other - Const lvalue reference of copied BinaryStructuredDataEngine class.
         *
         * @attention Need to check existence of data after use this constructor.
         */
        BinaryStructuredDataEngine (const BinaryStructuredDataEngine & /*other*/) noexcept;

        /**
         * @fn BinaryStructuredDataEngine::BinaryStructuredDataEngine (BinaryStructuredDataEngine &&) noexcept;
         * @brief Move assignment constructor of BinaryStructuredDataEngine class.
         * @param [in] other - Rvalue reference of moved BinaryStructuredDataEngine class.
         */
        BinaryStructuredDataEngine (BinaryStructuredDataEngine && /*other*/) noexcept;

        /**
         * @fn template <DATA_ENDIAN_TYPE Endian, typename Type>
         * bool BinaryStructuredDataEngine::AssignData (const Type *, const uint16_t * const, const uint16_t) noexcept;
         * @brief Method that assign binary data into internal state of structured data.
         * @tparam [in] Endian - Endian of input data. Default: Local System Type (DATA_SYSTEM_ENDIAN).
         * @tparam [in] Type - Typename of copied structured data.
         * @tparam [in] memory - POD type structure for assignment.
         * @param [in] pattern - Array that contains the byte-pattern of inputted structured data.
         * @param [in] size - Size of the pattern array.
         * @return TRUE - if data assignment is successful, otherwise - FALSE.
         *
         * @note Input type MUST be a POD type.
         */
        template <DATA_ENDIAN_TYPE Endian = DATA_SYSTEM_ENDIAN, typename Type>
        bool AssignData (const Type* memory, const uint16_t* const pattern, const uint16_t size) noexcept
        {
            static_assert(is_pod_type<Type>::value == true, "It is not possible to use not POD type for this method.");
            if (memory == nullptr) { return false; }

            const std::size_t bytes = static_cast<std::size_t>(std::accumulate(pattern, pattern + size, 0));
            if (bytes != sizeof(Type)) { return false; }

            data = BinaryDataEngine(bytes, STRUCTURED_DATA_HANDLING_MODE, DATA_BIG_ENDIAN);
            if (data == false) { return false; }

            if (data.AssignData(memory, 1) == false) {
                Clear();
                return false;
            }

            dataPattern = system::allocMemoryForArray<uint16_t>(size, pattern, size * sizeof(uint16_t));
            if (dataPattern == nullptr) {
                Clear();
                return false;
            }
            fieldsCount = size;

            const DATA_ENDIAN_TYPE inputEndian = (Endian == DATA_SYSTEM_ENDIAN) ? BinaryDataEngine::system_endian : Endian;
            if (inputEndian != dataEndianType) {
                // In this place there is a hack.
                const DATA_ENDIAN_TYPE lastEndian = dataEndianType;
                SetDataEndianType(inputEndian);
                dataEndianType = lastEndian;
            }
            return true;
        }

        /**
         * @fn bool BinaryStructuredDataEngine::CreateTemplate (const uint16_t *, uint16_t) noexcept;
         * @brief Method that creates empty structured data template.
         * @param [in] pattern - Array that contains the byte-pattern of inputted structured data.
         * @param [in] size - Size of the byte-pattern array.
         * @return TRUE - if creating the data structure template successfully, otherwise - FALSE.
         */
        bool CreateTemplate (const uint16_t * /*pattern*/, uint16_t /*size*/) noexcept;

        /**
         * @fn inline DATA_ENDIAN_TYPE BinaryStructuredDataEngine::DataEndianType() const noexcept;
         * @brief Method that returns the endian type of stored data in BinaryStructuredDataEngine class.
         * @return DATA_LITTLE_ENDIAN(0x02) - if on the system little endian, otherwise - DATA_BIG_ENDIAN(0x01).
         */
        inline DATA_ENDIAN_TYPE DataEndianType(void) const noexcept { return dataEndianType; }

        /**
         * @fn void BinaryStructuredDataEngine::SetDataEndianType (DATA_ENDIAN_TYPE) noexcept;
         * @brief Method that changes endian type of stored data in BinaryStructuredDataEngine class.
         * @param [in] endian - New data endian type.
         * @return TRUE - if endian type is changed successfully, otherwise - FALSE.
         */
        void SetDataEndianType (DATA_ENDIAN_TYPE /*endian*/) noexcept;

        /**
         * @fn inline std::size_t BinaryStructuredDataEngine::ByteSize() const noexcept;
         * @brief Method that returns the size of structured data in bytes.
         * @return Size of stored structured data in bytes.
         */
        inline std::size_t ByteSize(void) const noexcept { return data.Size(); }

        /**
         * @fn inline std::size_t BinaryStructuredDataEngine::BitSize() const noexcept;
         * @brief Method that returns the size of structured data in bits.
         * @return Size of stored structured data in bits.
         */
        inline std::size_t BitSize(void) const noexcept { return data.BitsInformation().Length(); }

        /**
         * @fn template <DATA_ENDIAN_TYPE Endian, typename Type>
         * bool BinaryStructuredDataEngine::SetField (const uint16_t, const Type) const noexcept;
         * @brief Method that sets new value to the selected field of structured data.
         * @tparam [in] Endian - Endian of input data. Default: Local System Type (DATA_SYSTEM_ENDIAN).
         * @tparam [in] Type - Typename of copied structured data.
         * @param [in] fieldIndex - Index of field in structured data.
         * @param [in] value - Field value for assignment to selected field of structured data in specified data endian type.
         * @return TRUE - if value assignment is successful, otherwise - FALSE.
         *
         * @note Input type MUST be a POD type.
         */
        template <DATA_ENDIAN_TYPE Endian = DATA_SYSTEM_ENDIAN, typename Type>
        bool SetField (const uint16_t fieldIndex, const Type value) const noexcept
        {
            static_assert(is_pod_type<Type>::value == true, "It is not possible to use not POD type for this method.");

            if (fieldIndex < fieldsCount && sizeof(Type) == dataPattern[fieldIndex])
            {
                BinaryDataEngine sequence(DATA_MODE_DEFAULT, (Endian == DATA_SYSTEM_ENDIAN) ? BinaryDataEngine::system_endian : Endian);
                if (sequence.AssignData<Type>(&value, 1) == false) { return false; }
                sequence.SetDataEndianType(dataEndianType);  // Change data endian type to internal endian format.

                // Calculate byte offset to start byte in selected field.
                const std::size_t offset = static_cast<std::size_t>(std::accumulate(dataPattern.get(), dataPattern.get() + fieldIndex, 0));

                // Copy new field value.
                for (std::size_t idx = 0; idx < dataPattern[fieldIndex]; ++idx) {
                    *data.GetAt(offset + idx) = *sequence.GetAt(idx);
                }
                return true;
            }
            return false;
        }

        /**
         * @fn template <uint8_t Mode, DATA_ENDIAN_TYPE Endian>
         * BinaryDataEngine BinaryStructuredDataEngine::GetField (const uint16_t) const noexcept;
         * @brief Method that returns field value of structured data under selected index.
         * @tparam [in] Mode - Type of output data handling mode. Default: DATA_MODE_DEFAULT.
         * @tparam [in] Endian - Endian of output data. Default: Local System Type (DATA_SYSTEM_ENDIAN).
         * @param [in] fieldIndex - Index of field in structured data.
         * @return Field value under selected index in BinaryDataEngine format in specified data handling and endian types.
         *
         * @attention Need to check existence of data after use this method.
         */
        template <uint8_t Mode = DATA_MODE_DEFAULT, DATA_ENDIAN_TYPE Endian = DATA_SYSTEM_ENDIAN>
        BinaryDataEngine GetField (const uint16_t fieldIndex) const noexcept
        {
            if (fieldIndex < fieldsCount)
            {
                // Get index of first byte of selected field (Not consider the type of endian in which data are presented).
                const std::size_t byteIndex = static_cast<std::size_t>(std::accumulate(dataPattern.get(), dataPattern.get() + fieldIndex, 0));
                BinaryDataEngine result(Mode, dataEndianType);
                if (result.AssignData(data.Data() + byteIndex, data.Data() + byteIndex + dataPattern[fieldIndex]) == true)
                {
                    // Change data endian type to specified output endian format.
                    result.SetDataEndianType((Endian == DATA_SYSTEM_ENDIAN) ? BinaryDataEngine::system_endian : Endian);
                    return result;
                }
            }
            return BinaryDataEngine(Mode, Endian);
        }

        /**
         * @fn template <typename Type, uint8_t Mode>
         * Type BinaryStructuredDataEngine::GetSubField (const uint16_t, const uint16_t, const uint16_t) const noexcept;
         * @brief Method that returns subfield value of structured data under selected index from low to high bit order.
         * @tparam [in] Type - Typename of variable to which subfield will be converted.
         * @tparam [in] Mode - Type of output data handling mode (dependent or not). Default: DATA_MODE_DEPENDENT.
         * @param [in] fieldIndex - Index of field in structured data.
         * @param [in] bitIndex - Bit index in selected field of structured data.
         * @return Subfield value under selected index in selected format from low to high bit order.
         */
        template <typename Type, uint8_t Mode = DATA_MODE_DEPENDENT>
        Type GetSubField (const uint16_t fieldIndex, const uint16_t bitIndex, const uint16_t length) const noexcept
        {
            static_assert(common::is_supports_binary_operations<Type>::value == true &&
                          std::is_default_constructible<Type>::value == true,
                          "It is not possible for this method to use type without binary operators and default constructor.");

            if (fieldIndex < fieldsCount && bitIndex + length - 1 < dataPattern[fieldIndex] * 8 && length <= sizeof(Type) * 8)
            {
                Type result = { };
                for (uint16_t idx = 0; idx < length; ++idx)
                {
                    const std::size_t bitOffset = GetBitOffset<Mode>(fieldIndex, bitIndex + idx);
                    if (bitOffset != BinaryDataEngine::npos) {
                        result = static_cast<Type>((result << 1) | (data.BitsInformation().GetBitValue(bitOffset) == true ? 0x01 : 0x00));
                    }
                }
                return result;
            }
            return Type();
        }

        /**
         * @fn BinaryDataEngine BinaryStructuredDataEngine::GetFieldByReference (uint16_t) const noexcept;
         * @brief Method that returns field value of structured data under selected index by reference.
         * @param [in] fieldIndex - Index of field in structured data.
         * @return Field value under selected index with referenced data in BinaryDataEngine format.
         *
         * @note The field reference value is always returns in internal data endian type and DATA_MODE_DEFAULT data handling mode.
         *
         * @attention Need to check existence of data after use this method.
         */
        BinaryDataEngine GetFieldByReference (uint16_t /*fieldIndex*/) const noexcept;

        /**
         * @fn template <uint8_t Mode>
         * bool BinaryStructuredDataEngine::SetFieldBit (const uint16_t, const uint16_t, const bool) noexcept;
         * @brief Method that sets new bit value to the selected field bit offset of structured data.
         * @tparam [in] Mode - Type of input data handling mode (dependent or not). Default: DATA_MODE_DEPENDENT.
         * @param [in] fieldIndex - Index of field in structured data.
         * @param [in] bitIndex - Bit index in selected field of structured data.
         * @param [in] value - Bit value for assignment to selected field bit offset of structured data. Default: true.
         * @return TRUE - if value assignment is successful, otherwise - FALSE.
         */
        template <uint8_t Mode = DATA_MODE_DEPENDENT>
        bool SetFieldBit (const uint16_t fieldIndex, const uint16_t bitIndex, const bool value = true) noexcept
        {
            if (fieldIndex < fieldsCount && bitIndex < dataPattern[fieldIndex] * 8)
            {
                const std::size_t bitOffset = GetBitOffset<Mode>(fieldIndex, bitIndex);
                if (bitOffset != BinaryDataEngine::npos) {
                    data.BitsTransform().Set(bitOffset, value);
                    return true;
                }
            }
            return false;
        }

        /**
         * @fn template <uint8_t Mode>
         * bool BinaryStructuredDataEngine::GetFieldBit (const uint16_t, const uint16_t) const noexcept;
         * @brief Method that returns bit field value of structured data under selected indexes.
         * @tparam [in] Mode - Type of output data handling mode (dependent or not). Default: DATA_MODE_DEPENDENT.
         * @param [in] fieldIndex - Index of field in structured data.
         * @param [in] bitIndex - Bit index in selected field of structured data.
         * @return Boolean value that indicates about the value of the selected bit.
         *
         * @warning Method always returns value 'false' if the index is out-of-range.
         */
        template <uint8_t Mode = DATA_MODE_DEPENDENT>
        bool GetFieldBit (const uint16_t fieldIndex, const uint16_t bitIndex) const noexcept
        {
            if (fieldIndex < fieldsCount && bitIndex < dataPattern[fieldIndex] * 8)
            {
                const std::size_t bitOffset = GetBitOffset<Mode>(fieldIndex, bitIndex);
                if (bitOffset != BinaryDataEngine::npos) {
                    return data.BitsInformation().GetBitValue(bitOffset);
                }
            }
            return false;
        }

        /**
         * @fn std::optional<uint16_t> BinaryStructuredDataEngine::GetNonemptyFieldIndex (uint16_t, const uint16_t *, uint16_t) const noexcept;
         * @brief Method that returns index of the first field in the selected bit-pattern where at least one bit is set.
         * @param [in] start - Start index in pattern from which nonempty field will be searched. Default: 0.
         * @param [in] pattern - Array that contains the bit-pattern of data. Default: nullptr.
         * @param [in] size - Size of the bit-pattern array. Default: 0.
         * @return Index of first binary nonempty field, otherwise - '0xFFFF'.
         *
         * @note Method does not returns a value if an error occurred.
         * @note If a specific pattern is not passed into the method then will be used internal byte-pattern.
         *
         * @warning This method is always used DATA_MODE_INDEPENDENT data handling mode.
         */
        std::optional<uint16_t> GetNonemptyFieldIndex (uint16_t start = 0, const uint16_t * /*pattern*/ = nullptr, uint16_t /*size*/ = 0) const noexcept;

        /**
         * @fn std::pair<uint16_t, const uint16_t *> BinaryStructuredDataEngine::GetPattern (uint16_t) const noexcept;
         * @brief Method that returns the internal byte-pattern with length from the specified position.
         * @param [in] fieldIndex - Index of field from which the internal byte-pattern will be returned. Default: 0.
         * @return Internal byte-pattern with it's length from the specified position or nullptr if an error occurred.
         */
        std::pair<uint16_t, const uint16_t *> GetPattern (uint16_t /*fieldIndex*/ = 0) const noexcept;

        /**
         * @fn inline const BinaryDataEngine & BinaryStructuredDataEngine::Data() const noexcept;
         * @brief Method that returns the reference to the internal structured data.
         * @return Const lvalue reference of internal structured data in BinaryDataEngine format.
         */
        inline const BinaryDataEngine& Data(void) const noexcept { return data; }

        /**
         * @fn void BinaryStructuredDataEngine::Clear() noexcept;
         * @brief Method that clears the internal binary structured data.
         */
        void Clear(void) noexcept;

        /**
         * @fn void BinaryStructuredDataEngine::Reset() noexcept;
         * @brief Method that resets the internal state of BinaryStructuredDataEngine class to default state.
         */
        void Reset(void) noexcept;

        /**
         * @fn std::string BinaryStructuredDataEngine::ToFormattedString() const noexcept;
         * @brief Method that outputs internal binary structured data by pattern in formatted string.
         * @return STL string object with sequence of the bit character representation of stored structured data.
         *
         * @note Data is always outputs in DATA_BIG_ENDIAN endian type.
         */
        std::string ToFormattedString(void) const noexcept;


        /**
         * @fn inline operator BinaryStructuredDataEngine::bool() const noexcept;
         * @brief Operator that returns the internal state of BinaryStructuredDataEngine class.
         * @return TRUE - if BinaryStructuredDataEngine class is not empty, otherwise - FALSE.
         */
        inline operator bool(void) const noexcept { return data == true; }

        /**
         * @fn BinaryStructuredDataEngine & BinaryStructuredDataEngine::operator= (const BinaryStructuredDataEngine &) noexcept;
         * @brief Copy assignment operator of BinaryStructuredDataEngine class.
         * @param [in] other - Const lvalue reference of copied BinaryStructuredDataEngine class.
         * @return Lvalue reference of copied BinaryStructuredDataEngine class.
         *
         * @attention Need to check existence of data after use this operator.
         */
        BinaryStructuredDataEngine & operator= (const BinaryStructuredDataEngine & /*other*/) noexcept;

        /**
         * @fn BinaryStructuredDataEngine & BinaryStructuredDataEngine::operator= (BinaryStructuredDataEngine &&) noexcept;
         * @brief Move assignment operator of BinaryStructuredDataEngine class.
         * @param [in] other - Rvalue reference of moved BinaryStructuredDataEngine class.
         * @return Lvalue reference of moved BinaryStructuredDataEngine class.
         */
        BinaryStructuredDataEngine & operator= (BinaryStructuredDataEngine && /*other*/) noexcept;

        /**
         * @fn friend inline std::ostream & operator<< (std::ostream &, const BinaryStructuredDataEngine &) noexcept;
         * @brief Operator that outputs internal binary structured data in binary string format.
         * @param [in,out] stream - Reference of the output stream engine.
         * @param [in] engine - Const lvalue reference of the BinaryStructuredDataEngine class.
         * @return Lvalue reference of the inputted STL std::ostream class.
         *
         * @note Data is always outputs in DATA_BIG_ENDIAN endian type.
         */
        friend inline std::ostream& operator<< (std::ostream& stream, const BinaryStructuredDataEngine& engine) noexcept
        {
            try
            {
                if (engine.data.IsEmpty() == false)
                {
                    stream.unsetf(std::ios_base::boolalpha);
                    uint16_t patternBlock = 0, blockBitCount = 0;

                    if (engine.DataEndianType() == DATA_BIG_ENDIAN)
                    {
                        for (std::size_t idx = 0; idx < engine.data.BitsInformation().Length(); ++idx)
                        {
                            if (idx % 8 == 0 && blockBitCount != 0) { stream << ' '; }
                            stream << engine.data.BitsInformation().Test(idx);
                            if (++blockBitCount == engine.dataPattern[patternBlock] * 8) {
                                if (++patternBlock == engine.fieldsCount) { break; }
                                stream << "    ";
                                blockBitCount = 0;
                            }
                        }
                    }
                    else  // If data endian type is DATA_LITTLE_ENDIAN.
                    {
                        std::size_t commonBitCount = 0;  // Count of all outputted bits.
                        std::size_t bitOffset = 0;  // Start bit offset in each bit field.
                        // Iterate over all fields of structured data.
                        for (uint16_t field = 0; field < engine.fieldsCount; ++field)
                        {
                            // Select required byte offset in each field.
                            for (int32_t byteOffset = engine.dataPattern[field] - 1; byteOffset >= 0; --byteOffset)
                            {
                                // Calculate bit offset to start bit in selected byte.
                                const std::size_t offset = bitOffset + static_cast<uint32_t>(byteOffset * 8);
                                // Iterate over all bits in selected byte.
                                for (std::size_t idx = offset; idx < offset + 8; ++idx)
                                {
                                    if (commonBitCount++ % 8 == 0 && blockBitCount != 0) { stream << ' '; }
                                    stream << engine.data.BitsInformation().Test(idx);
                                    if (++blockBitCount == engine.dataPattern[patternBlock] * 8) {
                                        if (++patternBlock == engine.fieldsCount) { break; }
                                        stream << "    ";
                                        blockBitCount = 0;
                                    }
                                }
                            }
                            bitOffset += engine.dataPattern[field] * 8;
                        }
                    }
                }
            }
            catch (const std::ios_base::failure& /*err*/) { }
            return stream;
        }

        /**
         * @fn friend inline BinaryStructuredDataEngine BinaryStructuredDataEngine::operator& (const BinaryStructuredDataEngine &, const BinaryStructuredDataEngine &) noexcept;
         * @brief Logical bitwise AND operator that transforms internal binary structured data.
         * @param [in] left - Const lvalue reference of the BinaryStructuredDataEngine class as left operand.
         * @param [in] right - Const lvalue reference of the BinaryStructuredDataEngine class as right operand.
         * @return New temporary object of transformed (by operation AND) BinaryStructuredDataEngine class.
         *
         * @note If operands have different endian and mode then result BinaryStructuredDataEngine will have endian and mode of the left operand.
         * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LESSER length (DATA_MODE_UNSAFE_OPERATOR mode).
         *
         * @attention Result BinaryStructuredDataEngine class is always processing in data mode type of left operand.
         * @attention The correct result can only be obtained if the data dependent modes are the same.
         */
        friend inline BinaryStructuredDataEngine operator& (const BinaryStructuredDataEngine& left, const BinaryStructuredDataEngine& right) noexcept
        {
            BinaryStructuredDataEngine result(left);
            if (result == true) {
                result.data &= right.data;
            }
            return result;
        }

        /**
         * @fn friend inline BinaryStructuredDataEngine BinaryStructuredDataEngine::operator| (const BinaryStructuredDataEngine &, const BinaryStructuredDataEngine &) noexcept;
         * @brief Logical bitwise OR operator that transforms internal binary structured data.
         * @param [in] left - Const lvalue reference of the BinaryStructuredDataEngine class as left operand.
         * @param [in] right - Const lvalue reference of the BinaryStructuredDataEngine class as right operand.
         * @return New temporary object of transformed (by operation OR) BinaryStructuredDataEngine class.
         *
         * @note If operands have different endian and mode then result BinaryStructuredDataEngine will have endian and mode of the left operand.
         * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LONGER length (DATA_MODE_UNSAFE_OPERATOR mode).
         *
         * @attention Result BinaryStructuredDataEngine class is always processing in data mode type of left operand.
         * @attention The correct result can only be obtained if the data dependent modes are the same.
         */
        friend inline BinaryStructuredDataEngine operator| (const BinaryStructuredDataEngine& left, const BinaryStructuredDataEngine& right) noexcept
        {
            BinaryStructuredDataEngine result(left);
            if (result == true) {
                result.data |= right.data;
            }
            return result;
        }

        /**
         * @fn friend inline BinaryStructuredDataEngine BinaryStructuredDataEngine::operator^ (const BinaryStructuredDataEngine &, const BinaryStructuredDataEngine &) noexcept;
         * @brief Logical bitwise XOR operator that transforms internal binary structured data.
         * @param [in] left - Const lvalue reference of the BinaryStructuredDataEngine class as left operand.
         * @param [in] right - Const lvalue reference of the BinaryStructuredDataEngine class as right operand.
         * @return New temporary object of transformed (by operation XOR) BinaryStructuredDataEngine class.
         *
         * @note If operands have different endian and mode then result BinaryStructuredDataEngine will have endian and mode of the left operand.
         * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LONGER length (DATA_MODE_UNSAFE_OPERATOR mode).
         *
         * @attention Result BinaryStructuredDataEngine class is always processing in data mode type of left operand.
         * @attention The correct result can only be obtained if the data dependent modes are the same.
         */
        friend inline BinaryStructuredDataEngine operator^ (const BinaryStructuredDataEngine& left, const BinaryStructuredDataEngine& right) noexcept
        {
            BinaryStructuredDataEngine result(left);
            if (result == true) {
                result.data ^= right.data;
            }
            return result;
        }
    };

}  // namespace types.


#endif  // PROTOCOL_ANALYZER_BINARY_STRUCTURED_DATA_ENGINE_HPP
