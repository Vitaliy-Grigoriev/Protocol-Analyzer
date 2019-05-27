// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#ifndef PROTOCOL_ANALYZER_INTEGER_VALUE_HPP
#define PROTOCOL_ANALYZER_INTEGER_VALUE_HPP

#include "BinaryDataEngine.hpp"  // BinaryDataEngine.


namespace analyzer::framework::common::types
{
    /**
     * @enum INTEGER_DIMENSION
     * @brief Dimension type of the stored value in IntegerValue class.
     */
    enum INTEGER_DIMENSION : uint8_t
    {
        INTEGER_8BIT_VALUE = 0x08,
        INTEGER_16BIT_VALUE = 0x10,
        INTEGER_32BIT_VALUE = 0x20,
        INTEGER_64BIT_VALUE = 0x40
    };


    /**
     * @class IntegerValue   IntegerValue.cpp   "include/framework/IntegerValue.cpp"
     * @brief This class defined the interface to work with integer values.
     */
    class IntegerValue
    {
    private:
        BinaryDataEngine storedValue;

        INTEGER_DIMENSION dimension;

        bool sign;

    public:
        IntegerValue (IntegerValue &&) = delete;
        IntegerValue (const IntegerValue &) = delete;
        IntegerValue & operator= (IntegerValue &&) = delete;
        IntegerValue & operator= (const IntegerValue &) = delete;

        /**
         * @brief Assign constructor of IntegerValue class.
         *
         * @param [in] dim - Dimension of integer value of INTEGER_DIMENSION type.
         * @param [in] isPositiveInteger - Flag that indicates about the sign of integer value. Default: TRUE (positive).
         */
        explicit IntegerValue (INTEGER_DIMENSION /*dim*/, bool /*isPositiveInteger*/ = true) noexcept;

        /**
         * @brief 8-bit unsigned integer constructor of IntegerValue class.
         *
         * @param [in] value - Assignable 8-bit value.
         */
        explicit IntegerValue (uint8_t /*value*/) noexcept;

        /**
         * @brief 16-bit unsigned integer constructor of IntegerValue class.
         *
         * @param [in] value - Assignable 16-bit value.
         */
        explicit IntegerValue (uint16_t /*value*/) noexcept;

        /**
         * @brief 32-bit unsigned integer constructor of IntegerValue class.
         *
         * @param [in] value - Assignable 32-bit value.
         */
        explicit IntegerValue (uint32_t /*value*/) noexcept;

        /**
         * @brief 64-bit unsigned integer constructor of IntegerValue class.
         *
         * @param [in] value - Assignable 64-bit value.
         */
        explicit IntegerValue (uint64_t /*value*/) noexcept;

        /**
         * @brief 8-bit integer constructor of IntegerValue class.
         *
         * @param [in] value - Assignable 8-bit value.
         */
        explicit IntegerValue (int8_t /*value*/) noexcept;

        /**
         * @brief 16-bit integer constructor of IntegerValue class.
         *
         * @param [in] value - Assignable 16-bit value.
         */
        explicit IntegerValue (int16_t /*value*/) noexcept;

        /**
         * @brief 32-bit integer constructor of IntegerValue class.
         *
         * @param [in] value - Assignable 32-bit value.
         */
        explicit IntegerValue (int32_t /*value*/) noexcept;

        /**
         * @brief 64-bit integer constructor of IntegerValue class.
         *
         * @param [in] value - Assignable 64-bit value.
         */
        explicit IntegerValue (int64_t /*value*/) noexcept;

        // Unary arithmetic operator.
        IntegerValue operator-(void) const noexcept;   // unary -

        /**
         * @brief Bitwise complement operator.
         */
        IntegerValue operator~(void) noexcept;

        // Binary arithmetic operators.
        IntegerValue operator+ (const IntegerValue & /*other*/) const;
        IntegerValue operator- (const IntegerValue & /*other*/) const;
        IntegerValue operator* (const IntegerValue & /*other*/) const;
        IntegerValue operator/ (const IntegerValue & /*other*/) const;
        IntegerValue operator% (const IntegerValue & /*other*/) const;

        // Arithmetic assignment operators.
        IntegerValue & operator+= (const IntegerValue & /*other*/);
        IntegerValue & operator-= (const IntegerValue & /*other*/);
        IntegerValue & operator*= (const IntegerValue & /*other*/);
        IntegerValue & operator/= (const IntegerValue & /*other*/);
        IntegerValue & operator%= (const IntegerValue & /*other*/);

        // Increment and decrement operators.
        IntegerValue & operator++(void);     // pre-increment
        IntegerValue & operator--(void);     // pre-decrement
        IntegerValue operator++ (int32_t);   // post-increment
        IntegerValue operator-- (int32_t);   // post-decrement

        // Bitwise assignment operators.
        IntegerValue & operator^= (const IntegerValue & /*other*/);
        IntegerValue & operator&= (const IntegerValue & /*other*/);
        IntegerValue & operator|= (const IntegerValue & /*other*/);

        /**
         * @brief Bitwise left shift assignment operator.

         * @param [in] shift - Bit offset for direct left bit shift as right operand.
         */
        IntegerValue & operator<<= (std::size_t /*shift*/) noexcept;

        /**
         * @brief Bitwise right shift assignment operator.
         *
         * @param [in] shift - Bit offset for direct right bit shift as right operand.
         */
        IntegerValue & operator>>= (std::size_t /*shift*/) noexcept;

        // Relational operators.
        bool operator< (const IntegerValue & /*other*/) const noexcept;
        bool operator> (const IntegerValue & /*other*/) const noexcept;
        bool operator<= (const IntegerValue & /*other*/) const noexcept;
        bool operator>= (const IntegerValue & /*other*/) const noexcept;
        bool operator== (const IntegerValue & /*other*/) const noexcept;
        bool operator!= (const IntegerValue & /*other*/) const noexcept;

        /**
         * @brief Operator that outputs IntegerValue in string format.
         *
         * @param [in,out] stream - Reference of the output stream engine.
         * @param [in] value - Constant lvalue reference of the IntegerValue class.
         * @return Lvalue reference of the inputted STL std::ostream class.
         */
        friend inline std::ostream& operator<< (std::ostream& stream, const IntegerValue& value) noexcept
        {
            switch (static_cast<uint16_t>(value.dimension))
            {
                case INTEGER_8BIT_VALUE:
                    if (value.sign == false)
                        stream << value.storedValue.To8Bit().value();
                    else
                        stream << value.storedValue.ToU8Bit().value();
                    break;
                case INTEGER_16BIT_VALUE:
                    if (value.sign == false)
                        stream << value.storedValue.To16Bit().value();
                    else
                        stream << value.storedValue.ToU16Bit().value();
                    break;
                case INTEGER_32BIT_VALUE:
                    if (value.sign == false)
                        stream << value.storedValue.To32Bit().value();
                    else
                        stream << value.storedValue.ToU32Bit().value();
                    break;
                case INTEGER_64BIT_VALUE:
                    if (value.sign == false)
                        stream << value.storedValue.To64Bit().value();
                    else
                        stream << value.storedValue.ToU64Bit().value();
                    break;
            }
            return stream;
        }

        /**
         * @brief Default destructor of IntegerValue class.
         */
        ~IntegerValue(void) = default;
    };

}  // namespace types.

#endif  // PROTOCOL_ANALYZER_INTEGER_VALUE_HPP
