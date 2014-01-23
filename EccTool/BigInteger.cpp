//
//  BigInteger.cpp
//  EccTool
//
//  Created by Josh Strom on 1/14/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#include "BigInteger.h"

#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <exception>
#include <sstream>
#include <algorithm>

using namespace std;

BigInteger::BigInteger(string number)
    : _sign((!number.empty() && number[0] == '-') ? NEGATIVE : POSITIVE)
{
    // Negative number means that the string was not empty the first character was '-'
    //  Strip off the negative sign.
    if(_sign == NEGATIVE)
        number = number.substr(1, string::npos);
        
    // Since a single 8-bit byte can hold the data represented by two hex characters,
    //  size the source buffer to the ceiling of half the length of the number string.
    _source.resize(static_cast<unsigned int>(ceil(static_cast<float>(number.size()) / static_cast<float>(2))));
    
    // The iteration through the destination buffer will be reversed.
    auto destinationIterator = _source.rbegin();
    
    // Iterate through the string containing the number in reverse order, two digits at a time.
    unsigned int i = static_cast<unsigned int>(number.size()) - 1;
    for(; i >= 1 && i < number.size(); i -= 2)
    {
        // Because we are iterating backwards, the hex digit at [i-1] is the most significant 2 bits
        //  and the hex digit at [i] is the least significant two bits. Shift and OR them together
        //  to create a single byte.
        *destinationIterator = (GetValidHexDigit(number[i - 1]) << 4) | GetValidHexDigit(number[i]);
        ++destinationIterator;
    }
    
    // Handle the final character (this will only happen for numbers with an odd number of digits).
    if(i == 0)
    {
        // No bit shifting here as we only have the least significant two bits.
        *destinationIterator = GetValidHexDigit(number[0]);
    }
    
    // Remove any empty zero bytes from the beginning of the buffer.
    TrimPrefixZeros();
}

BigInteger& BigInteger::operator+=(const BigInteger& rhs)
{
    Add(rhs);
    return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& rhs)
{
    Subtract(rhs);
    return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger& rhs)
{
    Multiply(rhs);
    return *this;
}

// Prefix-increment.
//  Applies the change to the return value.
BigInteger& BigInteger::operator++()
{
    *this += 1;
    return *this;
}

//Postfix-increment.
//  Applies the change *after* returning the same value. Change "this", return a copy.
//  Note: the int param is a dummy param used to determine the difference between the prefix
//  and postfix versions.
BigInteger BigInteger::operator++(int)
{
    BigInteger copy = *this;
    ++*this;
    
    return copy;
}

// Prefix-decrement.
//  Applies the change to the return value.
BigInteger& BigInteger::operator--()
{
    *this -= 1;
    return *this;
}

//Postfix-decrement.
//  Applies the change *after* returning the same value. Change "this", return a copy.
//  Note: the int param is a dummy param used to determine the difference between the prefix
//  and postfix versions.
BigInteger BigInteger::operator--(int)
{
    BigInteger copy = *this;
    --*this;
    
    return copy;
}

BigInteger BigInteger::operator-() const
{
    BigInteger negated = *this;
    
    if(negated.IsZero())
        return negated;
    
    if(negated.GetSign() == POSITIVE)
    {
        negated._sign = NEGATIVE;
        return negated;
    }
    
    negated._sign = POSITIVE;
    return negated;
}

bool BigInteger::operator<(const BigInteger& rhs) const
{
    return (Compare(rhs) == -1);
}

bool BigInteger::operator>(const BigInteger& rhs) const
{
    return (Compare(rhs) == 1);
}

bool BigInteger::operator<=(const BigInteger& rhs) const
{
    return (Compare(rhs) <= 0);
}

bool BigInteger::operator>=(const BigInteger& rhs) const
{
    return (Compare(rhs) >= 0);
}

bool BigInteger::operator==(const BigInteger& other) const
{
    return (Compare(other) == 0);
}

bool BigInteger::operator!=(const BigInteger& other) const
{
    return !(*this == other);
}

const string BigInteger::ToString() const
{
    // Set up the stringstream for printing. We want output to be hex and
    //  the fill digit to be '0' (note that these settings are "sticky").
    stringstream ss;
    ss << hex << setfill('0');
    
    // Print all digits to the stream.
    //  The width must be 2 (example: 0x5 should print as "05").
    //  The cast is to turn it from a character to a number.
    for(auto digit : _source)
        ss << setw(2) << (static_cast<uint16_t>(digit) & 0xFF);
    return ss.str();
}

bool BigInteger::IsZero() const
{
    return ((_source.size() == 1) && (_source[0] == 0));
}

BigInteger::Sign BigInteger::GetSign() const
{
    if(IsZero())
        return POSITIVE;
    
    return _sign;
}

uint8_t BigInteger::GetValidHexDigit(char digit)
{
    // We need to convert this hex character in to an actual
    //  hex binary digit.
    // Hex digit 0-9 requires different handling than
    //  hex digit a-f
    
    // 0-9: subtract the '0' character to adjust: '0'=0, '1'=1, etc.
    // a-f: subtract the 'a' character and add 10: 'a'=11, 'b'=12, etc.
    digit = tolower(digit);
    if(digit >= '0' && digit <= '9')
        return static_cast<uint8_t>(digit - '0');
    if(digit >= 'a' && digit <= 'f')
        return static_cast<uint8_t>(digit - 'a' + 10);
    
    // Not a valid hex digit.
    stringstream ss;
    ss << "Invalid hex digit: " << digit;
    throw invalid_argument(ss.str());
}

void BigInteger::TrimPrefixZeros()
{
    // Remove any empty zero bytes from the beginning of the buffer.
    auto firstNonzero = find_if(_source.begin(), _source.end(), [] (uint8_t element) { return element != 0; });
    _source.erase(_source.begin(), firstNonzero);
    
    // If this trimming results in an empty buffer, place a single zero.
    if(_source.size() == 0)
        _source.push_back(static_cast<uint8_t>(0));
}

void BigInteger::Borrow(vector<uint8_t>::reverse_iterator segmentBegin, const vector<uint8_t>::reverse_iterator& segmentEnd) const
{
    // Search ahead for the first non-zero character in the buffer.
    //  For the borrow to work correctly, it must populate back across
    //  previous elements in the buffer.
    //  Example (in decimal): Borrow starting from the ones place of 1000 -> 099(10)
    while(segmentBegin != segmentEnd)
    {
        // Correct element found. Decrement and return.
        if(*segmentBegin > 0)
        {
            (*segmentBegin)--;
            return;
        }
        
        // Element equal to zero, we need to adjust for the borrow.
        *segmentBegin = 0xFF;
        segmentBegin++;
    }
}

int BigInteger::Compare(const BigInteger& other) const
{
    const Sign thisSign = GetSign();
    const Sign otherSign = other.GetSign();
    
    if(thisSign != otherSign)
    {
        return (thisSign == POSITIVE) ? 1 : -1;
    }
    
    auto handleNegative = [=] (int compareResult) {
        return (thisSign == POSITIVE) ? compareResult : -compareResult;
    };
    
    // Since there are no zero bytes at the beginnig of the arrays,
    //  if one is longer than the other, it is larger than the other.
    if(_source.size() < other._source.size())
        return handleNegative(-1);
    else if(_source.size() > other._source.size())
        return handleNegative(1);
    
    // Find the first element (from the left) which differs between this and other.
    // Use that element to determine which is greater.
    for(unsigned int i = 0; i < _source.size(); i++)
    {
        if(_source[i] == other._source[i])
            continue;
        return handleNegative((_source[i] < other._source[i]) ? -1 : 1);
    }
    
    // None were different. They are the same.
    return 0;
}

BigInteger& BigInteger::Add(const BigInteger& rhs)
{
    // The operation will be done as a long-hand addition.
    // Example:
    //     (topNumber)      12345   [the larger (or longer) of the two]
    //  (bottomNumber)    + 45678   [the smaller (or shorter) of the two] -- If both are the same length, we don't care.
    //                    -------
    //           (sum)      58023
    //
    // The "topNumber" and "bottomNumber" buffers will be acceessed with
    //  reverse iterators in order to add in the correct order.
    // A "carry" variable is also utilized.
    
    auto& topNumberBuffer = (_source.size() > rhs._source.size()) ? _source : rhs._source;
    auto& bottomNumberBuffer = (_source.size() > rhs._source.size()) ? rhs._source : _source;
    
    // Prepare the _source buffer to hold the sum.
    //  Since an addition operation requires at most (longest number + 1) digits. Insert enough
    //  elements at the beginning to ensure that the buffer is large enough.
    _source.insert(_source.begin(), (topNumberBuffer.size() + 1 - _source.size()), 0);
    auto sumBuffer = _source.rbegin();
    
    auto topNumber = topNumberBuffer.rbegin();
    auto bottomNumber = bottomNumberBuffer.rbegin();
    uint8_t carry = 0;
    
    // Addition can result in a number that is (maximum) one extra character
    //  than the largest of the two operands. The "sum" buffer will be reserved at this size.
    
    // Sum will be filled "backwards" compared to the other two operands and reversed at the end.
    //  This is to simplify the creation through use of push_back().
    //vector<uint8_t> sumBuffer;
    //sumBuffer.reserve(topNumberBuffer.size() + 1);
    
    while(topNumber != topNumberBuffer.rend())
    {
        // We will always have a top number (as we are looping over topNumberBuffer), however,
        //  the bottom number might have finished already. Use 0 if this is the case.
        uint8_t currentTop = *topNumber;
        uint8_t currentButtom = (bottomNumber != bottomNumberBuffer.rend()) ? *bottomNumber : 0;
        
        // Do the addition, then take the top 4 bytes for the carry and push the bottom 4 bytes as the sum.
        uint16_t currentSum = currentTop + currentButtom + (carry >> 4);
        carry = (currentSum & 0xFF00) >> 4;
        *sumBuffer = (currentSum & 0x00FF);
        
        // TopNumber and SumBuffer are both of the same size.
        topNumber++;
        sumBuffer++;
        
        // Bottom number can be shorter than top number. Make sure to not increment past end.
        if(bottomNumber != bottomNumberBuffer.rend())
            bottomNumber++;
    }
    
    // If there is any leftover carry, add it to the sum buffer (shift it since it is one place up).
    if(carry != 0)
        *sumBuffer = (carry >> 4);
    
    // The sum buffer may be too large (due to the insert operation at the beginning), remove extra.
    TrimPrefixZeros();
    
    return *this;
}

BigInteger& BigInteger::Multiply(const BigInteger& rhs)
{
    // The operation will be done as a long-hand multiplication.
    // Example:
    //     (topNumber)        123   [the larger of the two]
    //  (bottomNumber)    *    11   [the smaller of the two]
    //                    -------
    //      (products)        123   [multiply, digit-wise, each of the upper digits by the lower, ]
    //                    +  1230   [shifting left by 1 lower digit multiplication]
    //                    -------
    // (sum of products)     1353   [then add the result]
    
    // Determine the larger buffer, this must be the top number for efficiency.
    bool lhsBigger = *this > rhs;
    auto& topOperandBuffer = (lhsBigger) ? _source : rhs._source;
    auto& bottomOperandBuffer = (lhsBigger) ? rhs._source : _source;
    
    // The product accumulator is used to accumulate the multiple additions: 1 for each bottom operand digit.
    //  Reserve enough memory to hold 2*<largest operand> so no reallocation of this buffer is necessary.
    BigInteger productAccumulator = 0;
    productAccumulator._source.reserve(topOperandBuffer.size() * 2);
    
    // The individual product is used to hold the result of each multiplication of a
    //  bottom digit by all of the top digits. We will deal with this one mostly by its vector.
    //  We will with it backwards and reverse it at the end. Simplifies indexing.
    BigInteger individualProduct = 0;
    auto& individualProductBuffer = individualProduct._source;
    
    // This counter gets incremented for each bottom digit multiplied to track the shift in the
    //  addition.
    int individualProductStartingIndex = 0;
    
    // Outer loop: iterate over the bottom operand digits in reverse.
    auto bottomOperandIterator = bottomOperandBuffer.rbegin();
    while(bottomOperandIterator != bottomOperandBuffer.rend())
    {
        // Prepare the individual product buffer. it should be large enough to hold a full multiplication.
        //  It should be as large as necessary and filled with zero (it will never resize smaller).
        individualProductBuffer.resize(topOperandBuffer.size() * 2, 0);
        fill(individualProductBuffer.begin(), individualProductBuffer.end(), 0);
        
        // Save away and increment the starting point for the individual product buffer.
        int individualProductIndex = individualProductStartingIndex++;
        
        // Inner loop: iterate over the top operand digits in reverse for each bottom operand digit.
        auto topOperandIterator = topOperandBuffer.rbegin();
        while(topOperandIterator != topOperandBuffer.rend())
        {
            // Do the operation on a single digit.
            uint8_t currentTopOperand = *topOperandIterator;
            uint8_t currentBottomOperand = *bottomOperandIterator;
            uint16_t currentProduct = currentTopOperand * currentBottomOperand;
            
            // Split the short into a high-byte and low-byte.
            uint8_t lowByte = (currentProduct & 0x00FF);
            uint8_t highByte = (currentProduct & 0xFF00) >> 8;
            
            // Each multiplication can affect maximum of 3 digits: the target digit, the next (for the high-byte),
            //  and one more for addition overflow.
            uint16_t calculatedLowByte = individualProductBuffer[individualProductIndex] + lowByte;
            uint16_t calculatedHighByte = individualProductBuffer[individualProductIndex + 1] + highByte + ((calculatedLowByte & 0xFF00) >> 8);
            uint8_t calculatedOverflowByte = (calculatedHighByte & 0xFF00) >> 8;
            
            // Set all these.
            individualProductBuffer[individualProductIndex] = (calculatedLowByte & 0x00FF);
            individualProductBuffer[individualProductIndex + 1] = (calculatedHighByte & 0x00FF);
            individualProductBuffer[individualProductIndex + 2] = calculatedOverflowByte;
            
            // Incrment all iterators/indices.
            topOperandIterator++;
            individualProductIndex++;
        }
        
        // Prepare individual product for addition (fix byte order, remove extra zeros, etc.)
        reverse(individualProductBuffer.begin(), individualProductBuffer.end());
        individualProduct.TrimPrefixZeros();
        productAccumulator += individualProduct;
        
        ++bottomOperandIterator;
    }
    
    // The switch updates the "this" reference.
    _source.swap(productAccumulator._source);
    
    return *this;
}

BigInteger& BigInteger::Subtract(const BigInteger& rhs)
{
    // For now, we do not support subtraction resulting in negative numbers
    //  (supported elswhere).
    if(rhs > *this)
    {
        stringstream ss;
        ss << "Negative result of subtraction not supported. Attempted operation: ";
        ss << ToString() << " - " << rhs.ToString();
        throw invalid_argument(ss.str());
    }
    
    // The operation will be done as a long-hand subtraction.
    // Example:
    //     (topNumber)      42345   [the larger of the two]
    //  (bottomNumber)    - 15678   [the smaller of the two]
    //                    -------
    //    (difference)      2cccd
    // A difference buffer and a isBorrowing flag will keep track of results.
    
    // The buffer may get smaller, but will not get larger.
    //  Note that we are adding the bytes to the difference buffer "backwards"
    //  to simplify determining empty digits.
    vector<uint8_t> differenceBuffer;
    differenceBuffer.reserve(_source.size());
    
    // Iterating through the top and bottom backwards.
    auto lhsIterator = _source.rbegin();
    auto rhsIterator = rhs._source.rbegin();
    
    while(lhsIterator != _source.rend())
    {
        // Pull out the current top (aways there) and the current bottom (or zero if no more digits).
        uint8_t currentTop = *lhsIterator;
        uint8_t currentBottom = (rhsIterator != rhs._source.rend()) ? *rhsIterator : 0;
        
        // Do the subtraction in a signed variable. This may result in a negative number.
        int currentDifference = currentTop - currentBottom;
        
        // Borrow from the next if necessary.
        if(currentDifference < 0)
        {
            Borrow(lhsIterator + 1, _source.rend());
            currentDifference += 0x100;
        }
        
        // Put the (now guarenteed positive) difference, masked to ensure proper size
        //  in the difference buffer.
        differenceBuffer.push_back(static_cast<unsigned short>(currentDifference) & 0xFF);
        
        // Increment everything.
        lhsIterator++;
        
        if(rhsIterator != rhs._source.rend())
            rhsIterator++;
    }
    
    // Since we created the difference backwards, reverse it then set it to the buffer.
    reverse(differenceBuffer.begin(), differenceBuffer.end());
    _source = differenceBuffer;
    
    // Remove any empty zero bytes from the beginning of the buffer.
    TrimPrefixZeros();
    
    return *this;
}






















