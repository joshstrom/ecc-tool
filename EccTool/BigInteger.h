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
#include <tuple>

using namespace std;

class BigInteger
{
    
private:
    
    // Enum to indicate the sign of the number.
    enum Sign
    {
        NEGATIVE,
        POSITIVE
    };
    
    // Contains the binary representation of the number.
    vector<uint8_t> _magnitude;
    
    Sign _sign;
    
    // Converts a hex character to a binary hex digit.
    static uint8_t GetValidHexDigit(char digit);
    
    // Removes any unnecessary prefix 0x00 bytes from the beginning of the buffer.
    void TrimPrefixZeros();

	// Sets the BigInteger equal to zero.
	void SetZero();
    
    void Borrow(vector<uint8_t>::reverse_iterator segmentBegin, const vector<uint8_t>::reverse_iterator& segmentEnd) const;
    
    // Compares this with another BigInteger.
    //  Returns -1 if this < other
    //  Returns 0 if this == other
    //  Returns 1 if this > other
    int CompareTo(const BigInteger& other) const;
    
    // Compares the magitude (or absolute value) of this with another BigInteger.
    //  Returns -1 if abs(this) < abs(other)
    //  Returns 0 if abs(this) == abs(other)
    //  Returns 1 if abs(this) > abs(other)
    int CompareMagnitudeTo(const BigInteger& other) const;
    
    // Sets the source buffer to a given unsigned integral type.
    template<typename T>
    void SetSourceBuffer(T number, typename std::enable_if<std::is_unsigned<T>::value, bool>::type* = 0)
    {
        // Convert this type to its binary representation.
        //  Note: since integers are stored in x86 little endian, we need to reverse the byte order
        //  as BigInteger uses a big endian representation.
        auto convertedPtr = reinterpret_cast<uint8_t*>(&number);
        _magnitude = vector<uint8_t>(convertedPtr, convertedPtr + sizeof(T));
        reverse(_magnitude.begin(), _magnitude.end());
        
        // Remove any unnecessary zeros.
        TrimPrefixZeros();
    }

    // Returns POSITIVE if the number is zero or positive and NEGATIVE otherwise.
    Sign GetSign() const;
    
    // The number is zero if the source buffer contains one character (zero).
    bool IsZero() const;
    
    // Internal mathematical helper functions.
    BigInteger& Add(const BigInteger& rhs);
    BigInteger& Subtract(const BigInteger& rhs);
    BigInteger& Multiply(const BigInteger& rhs);
    
public:
    
    // Constructs a BigInteger with a string containing a hex number.
    //  Note that decimal numbers are not supported and no prefix/suffix should be added.
    BigInteger(string number);

    // Constructs a BigInteger from an unsigned integer.
    template<typename T>
    BigInteger(T number, typename std::enable_if<std::is_unsigned<T>::value, bool>::type* = 0)
        : _sign(POSITIVE)
    {
        SetSourceBuffer(number);
    }
    
    // Constructs a BigInteger from a signed integer. This is needed for a check against negative
    //  numbers.
    template<typename T>
    BigInteger(T number, typename std::enable_if<std::is_signed<T>::value, bool>::type* = 0)
        : _sign((number >= 0) ? POSITIVE : NEGATIVE)
    {
        // The sign is handled in in the initializer.
        number = abs(number);
        SetSourceBuffer(static_cast<typename make_unsigned<T>::type>(number));
    }

    // Destructor, copy constructor, move constructor, and operator= (both move and copy)
    //  use default implementations.
    
    // Unary <operation>= mathematical operations.
    BigInteger& operator+=(const BigInteger& rhs);
    BigInteger& operator-=(const BigInteger& rhs);
    BigInteger& operator*=(const BigInteger& rhs);
    BigInteger& operator/=(const BigInteger& rhs);
    
    // Increment/Decrement operators.
    BigInteger& operator++(); // Prefix-increment.
    BigInteger operator++(int); //Postfix-increment.
    BigInteger& operator--(); // Prefix-decrement.
    BigInteger operator--(int); //Postfix-decrement.
    
    // Negation operator.
    BigInteger operator-() const;
    
    // Comparison operators.
    bool operator<(const BigInteger& rhs) const;
    bool operator>(const BigInteger& rhs) const;
    bool operator<=(const BigInteger& rhs) const;
    bool operator>=(const BigInteger& rhs) const;
    bool operator==(const BigInteger& other) const;
    bool operator!=(const BigInteger& other) const;
    
    // Bitwise operators.
    BigInteger& operator<<=(int count);
    
    BigInteger& operator%=(const BigInteger& rhs);
    
    // Converts the BigInteger to its string representation in hex. It will print
    //  with all digits concatenated and no separators. The first digit may be left-padded
    //  with '0' if appropriate.
    const string ToString() const;    
};

// Binary addition operator with BigIntegers. Declared as free function by convention.
inline BigInteger operator+(BigInteger lhs, const BigInteger& rhs)
{
    lhs += rhs;
    return lhs;
}

// Binary subtraction operator with BigIntegers. Declared as free function by convention.
inline BigInteger operator-(BigInteger lhs, const BigInteger& rhs)
{
    lhs -= rhs;
    return lhs;
}

// Binary multiplication operator with BigIntegers. Declared as free function by convention.
inline BigInteger operator*(BigInteger lhs, const BigInteger& rhs)
{
    lhs *= rhs;
    return lhs;
}

// Binary modulus operator with BigIntegers. Declared as free function by convention.
inline BigInteger operator%(BigInteger lhs, const BigInteger& rhs)
{
    lhs %= rhs;
    return lhs;
}

// Overload of the absolute value function abs() for BigInteger.
inline BigInteger abs(const BigInteger& bigInteger)
{
    return (bigInteger >= 0) ? bigInteger : -bigInteger;
}

#endif /* defined(__EccTool__BigInteger__) */
