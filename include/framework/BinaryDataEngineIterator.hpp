// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#ifndef PROTOCOL_ANALYZER_BINARY_DATA_ENGINE_ITERATOR_HPP
#define PROTOCOL_ANALYZER_BINARY_DATA_ENGINE_ITERATOR_HPP

#include <iterator>


namespace analyzer::framework::common::types
{
    class BinaryDataEngine;

    /**
     * @class BitReferenceContainer BinaryDataEngineIterator.hpp "include/framework/BinaryDataEngineIterator.hpp"
     * @brief Class that contains referenced binary data and gives an interface to work with it.
     */
    class BitReferenceContainer
    {
    private:
        /**
         * @var const BinaryDataEngine & storedData;
         * @brief Const lvalue reference of the BinaryDataEngine owner class.
         */
        const BinaryDataEngine & storedData;
        /**
         * @var std::size_t startBitPosition;
         * @brief Start bit position in BinaryDataEngine data.
         */
        std::size_t startBitPosition = 0;
        /**
         * @var std::size_t endPosition;
         * @brief Length of the bit sequence in BinaryDataEngine data.
         */
        std::size_t sequenceLength = 0;

    public:
        BitReferenceContainer(void) = delete;
        BitReferenceContainer (BitReferenceContainer &&) = delete;
        BitReferenceContainer (const BitReferenceContainer &) = delete;
        BitReferenceContainer & operator= (BitReferenceContainer &&) = delete;
        BitReferenceContainer & operator= (const BitReferenceContainer &) = delete;

        /**
         * @fn explicit BitReferenceContainer::BitReferenceContainer (const BinaryDataEngine &, const std::size_t, const std::size_t) noexcept;
         * @brief Constructor of BitReferenceContainer class.
         * @param [in] owner - Const lvalue reference of BinaryDataEngine owner class.
         * @param [in] start - Start bit position in BinaryDataEngine data. Default: 0.
         * @param [in] length - Length of the bit sequence in BinaryDataEngine data. Default: 1.
         */
        explicit BitReferenceContainer (const BinaryDataEngine& owner, const std::size_t position = 0, const std::size_t length = 1) noexcept
                : storedData(owner), startBitPosition(position), size(length)
        { }

        /**
         * @fn BitReferenceContainer::~BitReferenceContainer(void) noexcept;
         * @brief Default destructor.
         */
        ~BitReferenceContainer(void) noexcept = default;

        /**
         * @fn inline void BitReferenceContainer::SetRange (const std::size_t, const std::size_t) noexcept;
         * @brief Method that sets new range of bits in BinaryDataEngine class.
         * @param [in] start - Start bit position in BinaryDataEngine data.
         * @param [in] length - Length of the bit sequence in BinaryDataEngine data. Default: 0.
         *
         * @note If input length equals zero then
         */
        inline void SetRange (const std::size_t position, const std::size_t length = 0) noexcept
        {
            startBitPosition = position;
            if (length != 0) {
                sequenceLength = length;
            }
        }

        /**
         * @fn inline std::size_t BitReferenceContainer::Length(void) const noexcept;
         * @brief Method that returns the length of the referenced bit sequence.
         * @return Length of the referenced bit sequence.
         */
        inline std::size_t Length(void) const noexcept { return sequenceLength; }

        /**
         * @fn bool BitReferenceContainer::operator[] (std::size_t) const noexcept;
         * @brief Operator that returns the value of bit under the specified index.
         * @param [in] index - Index of bit in the referenced bit sequence.
         * @return Boolean value that indicates about the value of the selected bit.
         */
        //bool operator[] (std::size_t index) const noexcept;

    };



    template <typename DataType>
    class BinaryDataEngineConstIterator
    {
        friend class BinaryDataEngine;

    private:
        BitReferenceContainer outputValue;
        typename DataType::const_iterator currentIt;
        std::size_t countOfBits = 0;
        std::size_t offsetStep = 0;

    public:
        using difference_type    =  std::ptrdiff_t;
        using iterator_category  =  std::random_access_iterator_tag;
        using value_type         =  BitReferenceContainer;
        using pointer            =  const value_type *;
        using reference          =  const value_type &;
        using const_iterator     =  BinaryDataEngineConstIterator<DataType>;


        BinaryDataEngineConstIterator<DataType>(void) = delete;

        BinaryDataEngineConstIterator<DataType> (typename DataType::const_iterator owner, const std::size_t count, const std::size_t step = 0) noexcept
                : outputValue(owner), currentIt(owner), countOfBits(count), offsetStep(step)
        { }


        bool operator!= (const const_iterator& other) const noexcept
        {
            return ((other.currentIt) != this->currentIt);
        }

        bool operator== (const const_iterator& other) const noexcept
        {
            return ((other.currentIt) == this->currentIt);
        }


        reference operator*(void) const noexcept
        {
            return *(this->currentIt);
        }

        pointer operator->(void) const noexcept
        {
            return &this->operator*();
        }


        const_iterator& operator++ (void)
        {
            if (++current == listIt->cend()) {
                current = (++listIt)->cbegin();
            }
            return *this;
        }

        const_iterator operator++ (int32_t)
        {
            auto old = *this;
            ++(*this);
            return old;
        }


        const_iterator& operator-- (void)
        {
            if (current == listIt->cbegin()) {
                current = (--listIt)->cend();
            }
            --current;
            return *this;
        }

        const_iterator operator-- (int32_t)
        {
            auto old = *this;
            --(*this);
            return old;
        }
    };


}  // namespace types.


#endif  // PROTOCOL_ANALYZER_BINARY_DATA_ENGINE_ITERATOR_HPP
