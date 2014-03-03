//
//  Utilities.h
//  EccTool
//
//  Created by Josh Strom on 2/9/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
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
