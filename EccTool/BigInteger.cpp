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

using namespace std;

BigInteger::BigInteger(string number)
{
    // Since a single 8-bit byte can hold the data represented by two hex characters,
    //  size the source buffer to the ceiling of half the length of the number string.
    _source.resize(ceil(static_cast<float>(number.size()) / static_cast<float>(2)));
    
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
}

BigInteger& BigInteger::operator+=(const BigInteger& rhs)
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
    
    auto topNumber = topNumberBuffer.rbegin();
    auto bottomNumber = bottomNumberBuffer.rbegin();
    uint8_t carry = 0;
    
    // Addition can result in a number that is (maximum) one extra character
    //  than the largest of the two operands. The "sum" buffer will be reserved at this size.
    
    // Sum will be filled "backwards" compared to the other two operands and reversed at the end.
    //  This is to simplify the creation through use of push_back().
    vector<uint8_t> sumBuffer;
    sumBuffer.reserve(topNumberBuffer.size() + 1);

    while(topNumber != topNumberBuffer.rend())
    {
        // We will always have a top number (as we are looping over topNumberBuffer), however,
        //  the bottom number might have finished already. Use 0 if this is the case.
        auto currentTop = *topNumber;
        auto currentButtom = (bottomNumber != bottomNumberBuffer.rend()) ? *bottomNumber : 0;
        
        // Do the addition, then take the top 4 bytes for the carry and push the bottom 4 bytes as the sum.
        uint16_t currentSum = currentTop + currentButtom + carry;
        carry = (currentSum & 0xFF00) >> 4;
        sumBuffer.push_back(currentSum & 0x00FF);
        
        // Increment all iterators.
        topNumber++;
        bottomNumber++;
    }
    
    // Since we created the sum backwards, reverse it then set it to the buffer.
    reverse(sumBuffer.begin(), sumBuffer.end());
    _source = sumBuffer;
    
    return *this;
}

// Prefix-increment.
//  Applies the change to the return value.
BigInteger& BigInteger::operator++()
{
    *this += static_cast<uint8_t>(1);
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

uint8_t BigInteger::GetValidHexDigit(char digit) const
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































