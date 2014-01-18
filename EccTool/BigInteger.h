//
//  BigInteger.h
//  EccTool
//
//  Created by Josh Strom on 1/14/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#ifndef __EccTool__BigInteger__
#define __EccTool__BigInteger__

#include <iostream>
#include <vector>
#include <string>
#include <stdint.h>

using namespace std;

class BigInteger
{
private:
    vector<uint8_t> _source;
    
    // Converts a hex character to a binary hex digit.
    uint8_t GetValidHexDigit(char digit) const;
    
public:
    
    // Constructs a BigInteger with a string containing a hexadecimal number.
    //  Note: the "0x" notation to indicate a hex number rather than a decimal number
    //  is not supported at this time. Just provide the number and only a number in hex.
    BigInteger(string number);
    
    // Constructs a BigInteger from an unsigned integer.
    template<typename T>
    BigInteger(T number, typename std::enable_if<std::is_unsigned<T>::value, bool>::type* = 0)
    {
        // Convert this type to its binary representation.
        //  Note: since integers are stored in x86 little endian, we will eventually need to reverse
        //  the byte order. However, for now, leave as little endian.
        auto convertedPtr = reinterpret_cast<uint8_t*>(&number);
        _source = vector<uint8_t>(convertedPtr, convertedPtr + sizeof(T));
        
        // Since any empty "zero" portions of the number are at the end of the buffer
        //  (due to the little endian byte order), iterate bacwards until the first non-zero
        //  byte is found, then resize the array. Doing it in this order lets us avoid extra reallocation.
        int i = static_cast<int>(_source.size() - 1);
        for(; i >= 0; i--)
        {
            if(_source[i] != 0)
                break;
        }
        _source.resize(i + 1); // Add 1 to convert from index back to length.
        
        // Fix the endianness.
        reverse(_source.begin(), _source.end());
    }

    
    // Destructor, copy constructor, move constructor, and operator= (both move and copy)
    //  use default implementations.
    ~BigInteger() = default;
    BigInteger(const BigInteger& rhs) = default;
    BigInteger(BigInteger&& rhs) = default;
    BigInteger& operator=(const BigInteger& rhs) = default;
    BigInteger& operator=(BigInteger&& rhs) = default;
    
    // Addition/subtraction operations.
    BigInteger& operator+=(const BigInteger& rhs);
    
    // Increment/Decrement operators.
    BigInteger& operator++(); // Prefix-increment.
    BigInteger operator++(int); //Postfix-increment.
    
    // Converts the BigInteger to its string representation in hex. It will print
    //  with all digits concatenated and no separators. The first digit may be left-padded
    //  with '0' if appropriate.
    const string ToString() const;
    
};

// Addition operator with BigInteger. Declared as free function by convention.
inline BigInteger operator+(BigInteger lhs, const BigInteger& rhs)
{
    lhs += rhs;
    return lhs;
}


#endif /* defined(__EccTool__BigInteger__) */
