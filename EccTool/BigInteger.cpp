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

// Constructs a BigInteger with a string containing a hexadecimal number.
//  Note: the "0x" notation to indicate a hex number rather than a decimal number
//  is not supported at this time. Just provide the number and only a number in hex.
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
        // Parse each character into its binary equivalent.
        uint8_t leastSignificantBits = GetValidHexDigit(number[i]);
        uint8_t mostSignificantBits = GetValidHexDigit(number[i - 1]);
        
        // Join the digits (using bit shifting) in to a single byte.
        uint8_t combinedDigit = mostSignificantBits;
        combinedDigit <<= 4;
        combinedDigit += leastSignificantBits;
        
        // Place in the destination buffer.
        *destinationIterator = combinedDigit;
        ++destinationIterator;
    }
    
    // Handle the final character (this will only happen for numbers with an odd-number of digits).
    if(i == 0)
    {
        *destinationIterator = GetValidHexDigit(number[0]);
    }
}

// Converts the BigInteger to its string representation in hex. It will print
//  with all digits concatenated and no separators. The first digit may be left-padded
//  with '0' if appropriate.
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