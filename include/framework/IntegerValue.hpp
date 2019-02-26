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
        INTEGER_64BIT_VALUE = 0x40,
        INTEGER_128BIT_VALUE = 0x80
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
         * @brief Reference 8-bit unsigned integer constructor of IntegerValue class.
         *
         * @param [in] dim - Dimension of integer value of INTEGER_DIMENSION type.
         */
        IntegerValue (uint8_t & /*value*/) noexcept;


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
         * @brief Default destructor of IntegerValue class.
         */
        ~IntegerValue(void) = default;
    };
}  // namespace types.

#endif  // PROTOCOL_ANALYZER_INTEGER_VALUE_HPP
