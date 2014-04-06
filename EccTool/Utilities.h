//
//  The MIT License (MIT)
//
//  Copyright (c) 2014 Joshua Strom
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.
//
#ifndef __EccTool__Utilities__
#define __EccTool__Utilities__

#include <iostream>
#include <vector>
#include <string>
#include <stdint.h>


using namespace std;

namespace utilities
{
    // Converts a vector of bytes to a hexadecimal string representation.
    string BytesToHexString(const vector<uint8_t>& bytes);

    // Converts a hexadecimal string it its binary representation.
    vector<uint8_t> HexStringToBytes(string byteString);
    
    // Log the povided message to console when built in debug mode.
    void DebugLog(const string& message);
    void DebugLog(const char* message);
}

// Operators go outside of the namespace.

// Stream writing operator for a vector of bytes.
std::ostream& operator<<(std::ostream& os, const std::vector<uint8_t>& bytes);

#endif /* defined(__EccTool__Utilities__) */
