//
//  Utilities.cpp
//  EccTool
//
//  Created by Josh Strom on 2/9/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#include "Utilities.h"

#include <sstream>
#include <iomanip>
#include <string>
#include <math.h>

// Converts a string character into the ehx byte it represents.
uint8_t GetValidHexDigit(char digit)
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

string utilities::BytesToHexString(const vector<uint8_t>& bytes)
{
    stringstream ss;
    ss << bytes;
    
    return ss.str();
}

vector<uint8_t> utilities::HexStringToBytes(string byteString)
{
    // Strip spaces.
    byteString.erase(std::remove_if(byteString.begin(), byteString.end(), ::isspace), byteString.end());
    
    // Since a single 8-bit byte can hold the data represented by two hex characters,
    //  size the buffer to the ceiling of half the length of the number string.
    vector<uint8_t> bytes(static_cast<unsigned int>(ceil(static_cast<float>(byteString.size()) / static_cast<float>(2))));
    
    // The iteration through the destination buffer will be reversed.
    int destinationIndex = static_cast<int>(bytes.size()) - 1;
    
    // Iterate through the string containing the number in reverse order, two digits at a time.
    unsigned int i = static_cast<unsigned int>(byteString.size()) - 1;
    for(; i >= 1 && i < byteString.size(); i -= 2)
    {
        // Because we are iterating backwards, the hex digit at [i-1] is the most significant 2 bits
        //  and the hex digit at [i] is the least significant two bits. Shift and OR them together
        //  to create a single byte.
        bytes[destinationIndex] = (GetValidHexDigit(byteString[i - 1]) << 4) | GetValidHexDigit(byteString[i]);
        destinationIndex--;
    }
    
    // Handle the final character (this will only happen for numbers with an odd number of digits).
    if(i == 0)
    {
        // No bit shifting here as we only have the least significant two bits.
        bytes[destinationIndex] = GetValidHexDigit(byteString[0]);
    }

    return bytes;
}

std::ostream& operator<<(std::ostream& os, const std::vector<uint8_t>& bytes)
{
    stringstream ss;
    ss << hex << setfill('0');
    
    // Print all digits to the stream.
    //  The width must be 2 (example: 0x5 should print as "05").
    //  The cast is to turn it from a character to a number.
    for(auto digit : bytes)
        ss << setw(2) << (static_cast<uint16_t>(digit) & 0xFF);
    
    os << ss.str();
    
    return os;
}