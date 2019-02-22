// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
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
 */
#define STRUCTURED_DATA_HANDLING_MODE   (DATA_MODE_INDEPENDENT | DATA_MODE_SAFE_OPERATOR)

    /**
     * @class BinaryStructuredDataEngine   BinaryStructuredDataEngine.hpp   "include/framework/BinaryStructuredDataEngine.hpp"
     * @brief Main class of analyzer framework that contains binary structured data and gives an interface to work with it.
     *
     * @note This class is cross-platform.
     */
    template <typename BinaryDataType>
    class BinaryStructuredDataEngine
    {
    private:
        /**
         * @brief Variable that stores pointer to the binary structured data.
         *
         * @note This variable is always represents in STRUCTURED_DATA_HANDLING_MODE mode.
         */
        std::unique_ptr<BinaryDataType> data = nullptr;
        /**
         * @brief Flag that indicates that the data are constructed in StructuredDataConstructor.
         */
        bool isDataAllocated = false;
        /**
         * @brief Previous settings of data handling mode.
         */
        uint8_t previousDataHandlingMode = DATA_MODE_DEFAULT;
        /**
         * @brief Count of fields in byte-pattern.
         */
        uint16_t patternFieldsCount = 0;
        /**
         * @brief Pointer to the byte-pattern in which stored structured data are represented.
         */
        std::unique_ptr<const uint16_t[]> dataBytePattern = nullptr;


        /**
         * @class StructuredDataConstructor   BinaryStructuredDataEngine.hpp   "include/framework/BinaryStructuredDataEngine.hpp"
         * @brief This class defined the interface for construct the structured data.
         * 
         * @attention This class MUST BE initialized in constructors of owner class.
         */
        class StructuredDataConstructor
        {
            friend class BinaryStructuredDataEngine<BinaryDataType>;
            
        private:
            BinaryStructuredDataEngine<BinaryDataType> * const storedStructuredData = nullptr;
        
        public:
            StructuredDataConstructor (StructuredDataConstructor &&) = delete;
            StructuredDataConstructor (const StructuredDataConstructor &) = delete;
            StructuredDataConstructor & operator= (StructuredDataConstructor &&) = delete;
            StructuredDataConstructor & operator= (const StructuredDataConstructor &) = delete;

            /**
             * @brief Constructor of nested StructuredDataConstructor class.
             *
             * @param [in] owner - Constant pointer BinaryStructuredDataEngine owner class.
             */
            explicit StructuredDataConstructor (BinaryStructuredDataEngine<BinaryDataType>* const owner) noexcept
                : storedStructuredData(owner)
            { }

            /**
             * @brief Method that creates empty structured data template.
             *
             * @param [in] pattern - Byte-pattern of structured data.
             * @param [in] fieldsCount - Number of elements in pattern.
             * @param [in] validation - Flag that indicates whether the input pattern should be validated or not. Default: TRUE.
             * @return TRUE - if creating the data structure template successfully, otherwise - FALSE.
             *
             * @note Memory for the data will be allocated of size which calculated from pattern if data was not allocated previously.
             */
            bool CreateStructureTemplate (const uint16_t* const pattern, const uint16_t fieldsCount, const bool validation = true) noexcept
            {
                if (pattern == nullptr || fieldsCount == 0) { return false; }
                const std::size_t bytes = static_cast<std::size_t>(std::accumulate(pattern, pattern + fieldsCount, 0));
                if (bytes == 0) { return false; }

                if (validation == true && storedStructuredData->isDataAllocated == true)
                {
                    if (storedStructuredData->data->Size() != bytes) {
                        return false;
                    }
                }

                if (storedStructuredData->isDataAllocated == false)
                {
                    storedStructuredData->data = system::allocMemoryForObject<BinaryDataType>(bytes);
                    if (*storedStructuredData->data == false) {
                        return false;
                    }
                    storedStructuredData->isDataAllocated = true;
                }

                storedStructuredData->dataBytePattern = system::allocMemoryForArray<uint16_t>(fieldsCount, pattern, fieldsCount * sizeof(uint16_t));
                if (storedStructuredData->dataBytePattern == nullptr) {
                    return false;
                }
                storedStructuredData->patternFieldsCount = fieldsCount;
                return true;
            }

            /**
             * @brief Method that assign binary data into internal state of structured data.
             *
             * @tparam [in] Endian - Endian of stored data. Default: Local System Type (DATA_SYSTEM_ENDIAN).
             * @tparam [in] Mode - Type of input data handling mode. Default: DATA_MODE_DEPENDENT.
             * @tparam [in] Type - Typename of copied structured data.
             * @tparam [in] memory - POD type structure for assignment.
             * @param [in] count - Number of elements in inputted data of selected type.
             * @param [in] validation - Flag that indicates whether the input data should be validated or not. Default: TRUE.
             * @return TRUE - if data assignment is successful, otherwise - FALSE.
             *
             * @note Input type MUST be a POD type.
             */
            template <DATA_ENDIAN_TYPE Endian = DATA_SYSTEM_ENDIAN, uint8_t Mode = DATA_MODE_DEFAULT, typename Type>
            bool AssignData (const Type* const memory, const uint16_t count, const bool validation = true) noexcept
            {
                static_assert(is_pod_type<Type>::value == true, "It is not possible to use not POD type for this method.");
                if (memory == nullptr || count == 0) { return false; }

                const std::size_t size = count * sizeof(Type);
                if (validation == true && storedStructuredData->dataBytePattern != nullptr && storedStructuredData->patternFieldsCount != 0)
                {
                    const std::size_t bytes = static_cast<std::size_t>(std::accumulate(storedStructuredData->dataBytePattern.get(), storedStructuredData->dataBytePattern.get() + storedStructuredData->patternFieldsCount, 0));
                    if (bytes != size) {
                        return false;
                    }
                }

                storedStructuredData->data = system::allocMemoryForObject<BinaryDataType>(size, Mode, Endian);
                if (*storedStructuredData->data == false) {
                    return false;
                }
                storedStructuredData->isDataAllocated = true;

                if (storedStructuredData->data->AssignData(memory, count) == false) {
                    return false;
                }
                storedStructuredData->previousDataHandlingMode = Mode;
                storedStructuredData->data->SetDataModeType(STRUCTURED_DATA_HANDLING_MODE);
                return true;
            }

            /**
             * @brief Method that assign binary data into internal state of structured data.
             *
             * @tparam [in] Endian - Endian of stored data. Default: Local System Type (DATA_SYSTEM_ENDIAN).
             * @tparam [in] Mode - Type of input data handling mode. Default: DATA_MODE_DEPENDENT.
             * @tparam [in] Type - Typename of copied structured data.
             * @tparam [in] memory - POD type structure for assignment.
             * @param [in] count - Number of elements in inputted data of selected type.
             * @param [in] pattern - Byte-pattern of structured data.
             * @param [in] fieldsCount - Number of elements in pattern.
             * @param [in] validation - Flag that indicates whether the input data should be validated or not. Default: TRUE.
             * @return TRUE - if structured data assignment is successful, otherwise - FALSE.
             *
             * @note Input type MUST be a POD type.
             */
            template <DATA_ENDIAN_TYPE Endian = DATA_SYSTEM_ENDIAN, uint8_t Mode = DATA_MODE_DEFAULT, typename Type>
            bool AssignStructuredData (const Type* const memory, const uint16_t count, const uint16_t* const pattern, const uint16_t fieldsCount, const bool validation = true) noexcept
            {
                if (CreateStructureTemplate(pattern, fieldsCount, validation) == false) { return false; }
                if (AssignData<Endian, Mode>(memory, count, validation) == false) { return false; }
                return true;
            }

            /**
             * @brief Default destructor of nested StructuredDataConstructor class.
             */
            ~StructuredDataConstructor(void) = default;
        };


        /**
         * @brief Method that returns bit offset in selected field bit index of structured data.
         *
         * @tparam [in] Mode - Type of input data handling mode. Default: DATA_MODE_INDEPENDENT.
         * @param [in] fieldIndex - Index of field in structured data.
         * @param [in] bitIndex - Bit index in selected field of structured data.
         * @return Bit offset under selected field bit index of structured data.
         *
         * @note Method returns index 'BinaryDataEngine::npos' if selected index is out-of-range.
         */
        template <uint8_t Mode = DATA_MODE_INDEPENDENT>
        [[nodiscard]]
        std::size_t GetBitOffset (const uint16_t fieldIndex, const uint16_t bitIndex) const noexcept
        {
            if (fieldIndex < patternFieldsCount && bitIndex < dataBytePattern[fieldIndex] * 8)
            {
                // Calculate bit offset to the selected field.
                const std::size_t offset = static_cast<std::size_t>(std::accumulate(dataBytePattern.get(), dataBytePattern.get() + fieldIndex, 0)) * 8;
                if constexpr (Mode == DATA_MODE_INDEPENDENT) { return offset + bitIndex; }

                if (data->dataEndianType == DATA_LITTLE_ENDIAN) {
                    return offset + (bitIndex >> 3) * 8 - (bitIndex % 8) + 7;
                }
                else if (data->dataEndianType == DATA_BIG_ENDIAN) {
                    return offset + dataBytePattern[fieldIndex] * 8 - bitIndex - 1;
                }
                // If data endian type is DATA_REVERSE_BIG_ENDIAN.
                return offset + (dataBytePattern[fieldIndex] - (bitIndex >> 3) - 1) * 8 + (bitIndex % 8);
            }
            return BinaryDataType::npos;
        }

        StructuredDataConstructor constructor;

    public:
        // Disable copy assignment constructor of BinaryStructuredDataEngine class.
        BinaryStructuredDataEngine (const BinaryStructuredDataEngine<BinaryDataType> &) = delete;
        // Disable copy assignment operator of BinaryStructuredDataEngine class.
        BinaryStructuredDataEngine & operator= (const BinaryStructuredDataEngine<BinaryDataType> &) = delete;

        /**
         * @brief Constructor of BinaryStructuredDataEngine class with prepared structured data.
         *
         * @attention Need to check the existence of data after use this constructor by calling operator 'bool'.
         * @attention Data handling mode will be changed during the instance of BinaryStructuredDataEngine class is exists.
         */
        BinaryStructuredDataEngine<BinaryDataEngine>(void) noexcept
            : constructor(this)
        { }

        /**
         * @brief Constructor of BinaryStructuredDataEngine class with prepared structured data.
         *
         * @param [in] input - Lvalue reference of moved BinaryDataEngine class.
         * @param [in] pattern - Array that contains the byte-pattern in which stored structured data are represented.
         * @param [in] count - Number of elements in pattern.
         * @param [in] validation - Flag that indicates whether the input data should be validated or not. Default: TRUE.
         *
         * @attention Need to check the existence of data after use this constructor by calling operator 'bool'.
         * @attention Data handling mode will be changed during the instance of BinaryStructuredDataEngine class is exists.
         */
        BinaryStructuredDataEngine (BinaryDataType& input, const uint16_t* const pattern, const uint16_t count, const bool validation = true) noexcept
            : data(&input), constructor(this)
        {
            if (validation == true)
            {
                const std::size_t byteCount = static_cast<std::size_t>(std::accumulate(pattern, pattern + count, 0));
                if (byteCount != input.Size()) {
                    return;
                }
            }

            if (count != 0)
            {
                isDataAllocated = false;
                previousDataHandlingMode = data->dataModeType;
                data->SetDataModeType(STRUCTURED_DATA_HANDLING_MODE);
                patternFieldsCount = count;
                dataBytePattern.reset(pattern);
            }
        }

        /**
         * @brief Move assignment constructor of BinaryStructuredDataEngine class.
         *
         * @param [in] other - Rvalue reference of moved BinaryStructuredDataEngine class.
         */
        BinaryStructuredDataEngine (BinaryStructuredDataEngine<BinaryDataType>&& other) noexcept
        {
            if (other == true)
            {
                data = std::move(other.data);
                isDataAllocated = other.isDataAllocated;
                previousDataHandlingMode = other.previousDataHandlingMode;
                patternFieldsCount = other.patternFieldsCount;
                dataBytePattern = std::move(other.dataBytePattern);
            }
        }

        /**
         * @brief Method that returns the endian type of stored data in BinaryStructuredDataEngine class.
         *
         * @return The endian type of stored data.
         */
        inline DATA_ENDIAN_TYPE GetDataEndian(void) const noexcept { return data->dataEndianType; }

        /**
         * @brief Method that returns the constructor of BinaryStructuredDataEngine class.
         *
         * @return Lvalue reference of the StructuredDataConstructor class.
         */
        inline StructuredDataConstructor& Constructor(void) noexcept { return constructor; }

        /**
         * @brief Method that sets new value to the selected field of structured data.
         *
         * @tparam [in] Endian - Endian of input data. Default: Local System Type (DATA_SYSTEM_ENDIAN).
         * @tparam [in] Type - Typename of copied structured data.
         * @param [in] fieldIndex - Index of field in structured data.
         * @param [in] value - Field value for assignment to selected field of structured data in specified data endian type.
         * @return TRUE - if value assignment is successful, otherwise - FALSE.
         *
         * @note Input type MUST be a POD type.
         */
        template <DATA_ENDIAN_TYPE Endian = DATA_SYSTEM_ENDIAN, typename Type>
        bool SetField (const uint16_t fieldIndex, const Type value) noexcept
        {
            static_assert(is_pod_type<Type>::value == true, "It is not possible to use not POD type for this method.");

            if (fieldIndex < patternFieldsCount && sizeof(Type) == dataBytePattern[fieldIndex])
            {
                BinaryDataEngine sequence(DATA_MODE_DEFAULT, (Endian == DATA_SYSTEM_ENDIAN) ? BinaryDataEngine::system_endian : Endian);
                if (sequence.AssignData<Type>(&value, 1) == false) { return false; }
                sequence.SetDataEndianType(data->dataEndianType);  // Change data endian type to internal endian format.

                // Calculate byte offset to start byte in selected field.
                const std::size_t offset = static_cast<std::size_t>(std::accumulate(dataBytePattern, dataBytePattern + fieldIndex, 0));

                // Copy new field value.
                for (std::size_t idx = 0; idx < dataBytePattern[fieldIndex]; ++idx) {
                    *data->GetAt(offset + idx) = *sequence.GetAt(idx);
                }
                return true;
            }
            return false;
        }

        /**
         * @brief Method that returns field value of structured data under selected index.
         *
         * @tparam [in] Mode - Type of output data handling mode. Default: DATA_MODE_DEFAULT.
         * @tparam [in] Endian - Endian of output data. Default: Local System Type (DATA_SYSTEM_ENDIAN).
         * @param [in] fieldIndex - Index of field in structured data.
         * @return Field value under selected index in BinaryDataEngine format in specified data handling and endian types.
         */
        template <uint8_t Mode = DATA_MODE_DEFAULT, DATA_ENDIAN_TYPE Endian = DATA_SYSTEM_ENDIAN>
        std::optional<BinaryDataEngine> GetField (const uint16_t fieldIndex) const noexcept
        {
            if (fieldIndex < patternFieldsCount)
            {
                // Get index of first byte of selected field (Not consider the type of endian in which data are presented).
                const std::size_t byteIndex = static_cast<std::size_t>(std::accumulate(dataBytePattern.get(), dataBytePattern.get() + fieldIndex, 0));
                BinaryDataEngine result(Mode, data->dataEndianType);
                if (result.AssignData(data->Data() + byteIndex, data->Data() + byteIndex + dataBytePattern[fieldIndex]) == true)
                {
                    // Change data endian type to specified output endian format.
                    result.SetDataEndianType((Endian == DATA_SYSTEM_ENDIAN) ? BinaryDataEngine::system_endian : Endian);
                    return result;
                }
            }
            return std::nullopt;
        }

        /**
         * @brief Method that returns subfield value of structured data under selected index from low to high bit order.
         *
         * @tparam [in] Type - Typename of variable to which subfield will be converted.
         * @tparam [in] Mode - Type of output data handling mode (dependent or not). Default: DATA_MODE_DEPENDENT.
         * @param [in] fieldIndex - Index of field in structured data.
         * @param [in] bitIndex - Bit index in selected field of structured data.
         * @return Subfield value under selected index in selected format from low to high bit order.
         */
        template <typename Type, uint8_t Mode = DATA_MODE_DEPENDENT>
        std::optional<Type> GetSubField (const uint16_t fieldIndex, const uint16_t bitIndex, const uint16_t length) const noexcept
        {
            static_assert(common::is_supports_binary_operations<Type>::value == true &&
                          std::is_default_constructible<Type>::value == true,
                          "It is not possible for this method to use type without binary operators and default constructor.");

            if (fieldIndex < patternFieldsCount && bitIndex + length - 1 < dataBytePattern[fieldIndex] * 8 && length <= sizeof(Type) * 8)
            {
                Type result = { };
                for (uint16_t idx = 0; idx < length; ++idx)
                {
                    const std::size_t bitOffset = GetBitOffset<Mode>(fieldIndex, bitIndex + idx);
                    if (bitOffset != BinaryDataType::npos) {
                        result = static_cast<Type>((result << 1) | (data->bitStreamInformation.GetBitValue(bitOffset) == true ? 0x01 : 0x00));
                    }
                }
                return result;
            }
            return std::nullopt;
        }

        /**
         * @brief Method that returns field value of structured data under selected index by reference.
         *
         * @param [in] fieldIndex - Index of field in structured data.
         * @return Field value under selected index with referenced data in BinaryDataEngine format.
         *
         * @note The field reference value is always returns in internal data endian type and DATA_MODE_DEFAULT data handling mode.
         */
        std::optional<BinaryDataType> GetFieldByReference (uint16_t /*fieldIndex*/) const noexcept;

        /**
         * @brief Method that sets new bit value to the selected field bit offset of structured data.
         *
         * @tparam [in] Mode - Type of input data handling mode (dependent or not). Default: DATA_MODE_DEPENDENT.
         * @param [in] fieldIndex - Index of field in structured data.
         * @param [in] bitIndex - Bit index in selected field of structured data.
         * @param [in] value - Bit value for assignment to selected field bit offset of structured data. Default: true.
         * @return TRUE - if value assignment is successful, otherwise - FALSE.
         */
        template <uint8_t Mode = DATA_MODE_DEPENDENT>
        bool SetFieldBit (const uint16_t fieldIndex, const uint16_t bitIndex, const bool value = true) noexcept
        {
            if (*this == false) { return false; }

            if (fieldIndex < patternFieldsCount && bitIndex < dataBytePattern[fieldIndex] * 8)
            {
                const std::size_t bitOffset = GetBitOffset<Mode>(fieldIndex, bitIndex);
                if (bitOffset != BinaryDataType::npos) {
                    data->bitStremTransform.Set(bitOffset, value);
                    return true;
                }
            }
            return false;
        }

        /**
         * @brief Method that returns bit field value of structured data under selected indexes.
         *
         * @tparam [in] Mode - Type of output data handling mode (dependent or not). Default: DATA_MODE_DEPENDENT.
         * @param [in] fieldIndex - Index of field in structured data.
         * @param [in] bitIndex - Bit index in selected field of structured data.
         * @return Boolean value that indicates about the value of the selected bit.
         *
         * @warning Method always returns value 'false' if the index is out-of-range or data not allocated.
         */
        template <uint8_t Mode = DATA_MODE_DEPENDENT>
        bool GetFieldBit (const uint16_t fieldIndex, const uint16_t bitIndex) const noexcept
        {
            if (fieldIndex < patternFieldsCount && bitIndex < dataBytePattern[fieldIndex] * 8)
            {
                const std::size_t bitOffset = GetBitOffset<Mode>(fieldIndex, bitIndex);
                if (bitOffset != BinaryDataType::npos) {
                    return data->bitStreamInformation.GetBitValue(bitOffset);
                }
            }
            return false;
        }

        /**
         * @brief Method that returns index of the first field in the selected bit-pattern where at least one bit is set.
         *
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
         * @brief Method that returns the internal byte-pattern with length from the specified position.
         *
         * @param [in] fieldIndex - Index of field from which the internal byte-pattern will be returned. Default: 0.
         * @return Internal byte-pattern with it's length from the specified position or nullptr if an error occurred.
         */
        std::pair<uint16_t, const uint16_t *> GetPattern (uint16_t /*fieldIndex*/ = 0) const noexcept;

        /**
         * @brief Method that returns the reference to the internal structured data.
         *
         * @return Lvalue reference of BinaryDataType format.
         */
        inline BinaryDataType& Data(void) const noexcept { return *data; }

        /**
         * @brief Method that returns pointer to the internal structured data and release it.
         *
         * @return Lvalue pointer of BinaryDataType format.
         */
        inline BinaryDataType* Release(void) const noexcept
        {
            isDataAllocated = false;
            return data.release();
        }

        /**
         * @brief Method that outputs internal binary structured data by pattern in formatted string.
         *
         * @return STL string object with sequence of the bit character representation of stored structured data.
         *
         * @note Data is always outputs in DATA_BIG_ENDIAN endian type.
         */
        std::string ToFormattedString(void) const noexcept;

        /**
         * @brief Operator that returns the internal state of BinaryStructuredDataEngine class.
         *
         * @return TRUE - if BinaryStructuredDataEngine class is not empty, otherwise - FALSE.
         */
        inline operator bool(void) const noexcept { return (data != nullptr && *data == true && dataBytePattern != nullptr); }

        /**
         * @brief Move assignment operator of BinaryStructuredDataEngine class.
         *
         * @param [in] other - Rvalue reference of moved BinaryStructuredDataEngine class.
         * @return Lvalue reference of moved BinaryStructuredDataEngine class.
         */
        BinaryStructuredDataEngine& operator= (BinaryStructuredDataEngine<BinaryDataType>&& other) noexcept
        {
            if (this != &other && other == true)
            {
                data = std::move(other.data);
                isDataAllocated = other.isDataAllocated;
                previousDataHandlingMode = other.previousDataHandlingMode;
                patternFieldsCount = other.patternFieldsCount;
                dataBytePattern = std::move(other.dataBytePattern);
            }
            return *this;
        }

        /**
         * @brief Operator that outputs internal binary structured data in binary string format.
         *
         * @param [in,out] stream - Reference of the output stream engine.
         * @param [in] engine - Constant lvalue reference of the BinaryStructuredDataEngine class.
         * @return Lvalue reference of the inputted STL std::ostream class.
         *
         * @note Data is always outputs in DATA_BIG_ENDIAN endian type.
         */
        friend inline std::ostream& operator<< (std::ostream& stream, const BinaryStructuredDataEngine<BinaryDataType>& engine) noexcept
        {
            try
            {
                if (engine.data->IsEmpty() == false)
                {
                    stream.unsetf(std::ios_base::boolalpha);
                    uint16_t patternBlock = 0, blockBitCount = 0;

                    if (engine.DataEndianType() == DATA_BIG_ENDIAN)
                    {
                        for (std::size_t idx = 0; idx < engine.data.bitStreamInformation.Length(); ++idx)
                        {
                            if (idx % 8 == 0 && blockBitCount != 0) { stream << ' '; }
                            stream << engine.data->bitStreamInformation.Test(idx);
                            if (++blockBitCount == engine.dataBytePattern[patternBlock] * 8) {
                                if (++patternBlock == engine.patternFieldsCount) { break; }
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
                        for (uint16_t field = 0; field < engine.patternFieldsCount; ++field)
                        {
                            // Select required byte offset in each field.
                            for (int32_t byteOffset = engine.dataBytePattern[field] - 1; byteOffset >= 0; --byteOffset)
                            {
                                // Calculate bit offset to start bit in selected byte.
                                const std::size_t offset = bitOffset + static_cast<uint32_t>(byteOffset * 8);
                                // Iterate over all bits in selected byte.
                                for (std::size_t idx = offset; idx < offset + 8; ++idx)
                                {
                                    if (commonBitCount++ % 8 == 0 && blockBitCount != 0) { stream << ' '; }
                                    stream << engine.data->bitStreamInformation.Test(idx);
                                    if (++blockBitCount == engine.dataBytePattern[patternBlock] * 8) {
                                        if (++patternBlock == engine.patternFieldsCount) { break; }
                                        stream << "    ";
                                        blockBitCount = 0;
                                    }
                                }
                            }
                            bitOffset += engine.dataBytePattern[field] * 8;
                        }
                    }
                }
            }
            catch (const std::ios_base::failure& /*err*/) { }
            return stream;
        }

        /**
         * @brief Bitwise AND operator that transforms internal binary structured data.
         *
         * @param [in] left - Constant lvalue reference of the BinaryStructuredDataEngine class as left operand.
         * @param [in] right - Constant lvalue reference of the BinaryStructuredDataEngine class as right operand.
         * @return New temporary object of transformed (by operation AND) BinaryStructuredDataEngine class.
         *
         * @note If operands have different endian and mode then result BinaryStructuredDataEngine will have endian and mode of the left operand.
         * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LESSER length (DATA_MODE_UNSAFE_OPERATOR mode).
         *
         * @attention Result BinaryStructuredDataEngine class is always processing in data mode type of left operand.
         * @attention The correct result can only be obtained if the data dependent modes are the same.
         */
        friend inline BinaryStructuredDataEngine operator& (const BinaryStructuredDataEngine<BinaryDataType>& left, const BinaryStructuredDataEngine<BinaryDataType>& right) noexcept
        {
            BinaryStructuredDataEngine<BinaryDataType> result(left);
            if (result == true) {
                *result.data &= *right.data;
            }
            return result;
        }

        /**
         * @brief Bitwise OR operator that transforms internal binary structured data.
         *
         * @param [in] left - Constant lvalue reference of the BinaryStructuredDataEngine class as left operand.
         * @param [in] right - Constant lvalue reference of the BinaryStructuredDataEngine class as right operand.
         * @return New temporary object of transformed (by operation OR) BinaryStructuredDataEngine class.
         *
         * @note If operands have different endian and mode then result BinaryStructuredDataEngine will have endian and mode of the left operand.
         * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LONGER length (DATA_MODE_UNSAFE_OPERATOR mode).
         *
         * @attention Result BinaryStructuredDataEngine class is always processing in data mode type of left operand.
         * @attention The correct result can only be obtained if the data dependent modes are the same.
         */
        friend inline BinaryStructuredDataEngine operator| (const BinaryStructuredDataEngine<BinaryDataType>& left, const BinaryStructuredDataEngine<BinaryDataType>& right) noexcept
        {
            BinaryStructuredDataEngine<BinaryDataType> result(left);
            if (result == true) {
                *result.data |= *right.data;
            }
            return result;
        }

        /**
         * @brief Bitwise XOR operator that transforms internal binary structured data.
         *
         * @param [in] left - Constant lvalue reference of the BinaryStructuredDataEngine class as left operand.
         * @param [in] right - Constant lvalue reference of the BinaryStructuredDataEngine class as right operand.
         * @return New temporary object of transformed (by operation XOR) BinaryStructuredDataEngine class.
         *
         * @note If operands have different endian and mode then result BinaryStructuredDataEngine will have endian and mode of the left operand.
         * @note To improve the speed and less memory consumption, it is necessary that the left operand has data of a LONGER length (DATA_MODE_UNSAFE_OPERATOR mode).
         *
         * @attention Result BinaryStructuredDataEngine class is always processing in data mode type of left operand.
         * @attention The correct result can only be obtained if the data dependent modes are the same.
         */
        friend inline BinaryStructuredDataEngine operator^ (const BinaryStructuredDataEngine<BinaryDataType>& left, const BinaryStructuredDataEngine<BinaryDataType>& right) noexcept
        {
            BinaryStructuredDataEngine<BinaryDataType> result(left);
            if (result == true) {
                *result.data ^= *right.data;
            }
            return result;
        }

        /**
         * @brief Destructor of BinaryStructuredDataEngine class.
         */
        ~BinaryStructuredDataEngine(void) noexcept
        {
            patternFieldsCount = 0;
            if (data != nullptr && isDataAllocated == false)
            {
                data->SetDataModeType(previousDataHandlingMode);
                data.release();
                dataBytePattern.release();
            }
            else if (data != nullptr)  // If data are allocated in constructor.
            {
                data.reset(nullptr);
                dataBytePattern.reset(nullptr);
            }
        }
    };



    // Method that returns field value of structured data under selected index by reference.
    template <typename BinaryDataType>
    std::optional<BinaryDataType> BinaryStructuredDataEngine<BinaryDataType>::GetFieldByReference (const uint16_t fieldIndex) const noexcept
    {
        if (fieldIndex < patternFieldsCount)
        {
            // Get index of first byte of selected field (Not consider the type of endian in which data are presented).
            const std::size_t byteIndex = static_cast<std::size_t>(std::accumulate(dataBytePattern, dataBytePattern + fieldIndex, 0));
            BinaryDataType result(data->GetAt(byteIndex), dataBytePattern[fieldIndex], data->dataEndianType);
            return result;
        }
        return std::nullopt;
    }

    // Method that returns index of the first field in the selected bit-pattern where at least one bit is set.
    template <typename BinaryDataType>
    std::optional<uint16_t> BinaryStructuredDataEngine<BinaryDataType>::GetNonemptyFieldIndex (const uint16_t start, const uint16_t* const pattern, const uint16_t size) const noexcept
    {
        // Check inputted 'start' and 'size' variables in one condition.
        if (*this == false || start >= size) { return std::nullopt; }

        std::size_t offset = 0;  // Current bit offset.
        if (pattern != nullptr)
        {
            const std::size_t bitCount = static_cast<std::size_t>(std::accumulate(pattern, pattern + size, 0));
            if (bitCount != data->bitStreamInformation.Length()) { return std::nullopt; }

            if (start != 0) {
                offset = static_cast<std::size_t>(std::accumulate(pattern, pattern + start, 0));
            }

            for (uint16_t field = start; field < size; ++field)
            {
                if (data->bitStreamInformation.Any(offset, offset + pattern[field] - 1) == true) {
                    return field;
                }
                offset += pattern[field];
            }
        }
        else if (dataBytePattern != nullptr)  // In this case the internal byte-pattern is used.
        {
            if (start != 0) {
                offset = static_cast<std::size_t>(std::accumulate(dataBytePattern.get(), dataBytePattern.get() + start, 0) * 8);
            }

            for (uint16_t field = start; field < patternFieldsCount; ++field)
            {
                if (data->bitStreamInformation.Any(offset, offset + dataBytePattern[field] * 8 - 1) == true) {
                    return field;
                }
                offset += dataBytePattern[field] * 8;
            }
        }
        return std::numeric_limits<uint16_t>::max();
    }

    // Method that returns the internal byte-pattern with length from the specified position.
    template <typename BinaryDataType>
    std::pair<uint16_t, const uint16_t*> BinaryStructuredDataEngine<BinaryDataType>::GetPattern (const uint16_t fieldIndex) const noexcept
    {
        if (fieldIndex < patternFieldsCount) {
            return { patternFieldsCount - fieldIndex, &dataBytePattern[fieldIndex] };
        }
        return { 0, nullptr };
    }

    // Method that outputs internal binary structured data by pattern in string format.
    template <typename BinaryDataType>
    std::string BinaryStructuredDataEngine<BinaryDataType>::ToFormattedString(void) const noexcept
    {
        std::ostringstream result;
        if (data->IsEmpty() == false)
        {
            result.unsetf(std::ios_base::boolalpha);
            uint16_t patternBlock = 0, blockBitCount = 0;

            result << "\nField 1:   ";

            if (data->dataEndianType == DATA_BIG_ENDIAN)
            {
                for (std::size_t idx = 0; idx < data->bitStreamInformation.Length(); ++idx)
                {
                    if (idx % 8 == 0 && blockBitCount != 0) { result << ' '; }
                    result << data->bitStreamInformation.GetBitValue(idx);
                    if (++blockBitCount == dataBytePattern[patternBlock] * 8) {
                        if (++patternBlock == patternFieldsCount) { break; }
                        result << "\nField " << patternBlock + 1 << ":   ";
                        blockBitCount = 0;
                    }
                }
            }
            else  // If data endian type is DATA_LITTLE_ENDIAN.
            {
                std::size_t commonBitCount = 0;  // Count of all outputted bits.
                std::size_t bitOffset = 0;  // Start bit offset in each bit field.
                // Iterate over all fields of structured data.
                for (uint16_t field = 0; field < patternFieldsCount; ++field)
                {
                    // Select required byte offset in each field.
                    for (int32_t byteOffset = dataBytePattern[field] - 1; byteOffset >= 0; --byteOffset)
                    {
                        // Calculate bit offset to start bit in selected byte.
                        const std::size_t offset = bitOffset + static_cast<uint32_t>(byteOffset * 8);
                        // Iterate over all bits in selected byte.
                        for (std::size_t idx = offset; idx < offset + 8; ++idx)
                        {
                            if (commonBitCount++ % 8 == 0 && blockBitCount != 0) { result << ' '; }
                            result << data->bitStreamInformation.GetBitValue(idx);
                            if (++blockBitCount == dataBytePattern[patternBlock] * 8) {
                                if (++patternBlock == patternFieldsCount) { break; }
                                result << "\nField " << patternBlock + 1 << ":   ";
                                blockBitCount = 0;
                            }
                        }
                    }
                    bitOffset += dataBytePattern[field] * 8;
                }
            }
        }
        return result.str();
    }

}  // namespace types.

#endif  // PROTOCOL_ANALYZER_BINARY_STRUCTURED_DATA_ENGINE_HPP
