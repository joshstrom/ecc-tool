//
//  NativeCrypto.h
//  EccTool
//
//  Created by Josh Strom on 2/13/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#ifndef __EccTool__NativeCrypto__
#define __EccTool__NativeCrypto__

// This class provides an interface into system dependent crypto primitives.
// Compile-time preprocessing is used to select appropriate code for the operating system.

#include <vector>
#include <memory>

class NativeCrypto
{
public:
    
    // Derive a key of provided size with password and salt.
	static std::vector<uint8_t> DeriveKey(const std::vector<uint8_t>& password, const std::vector<uint8_t>& salt, size_t keySize);

    // Hash the provided data with the SHA 256 hash algorithm.
	static std::vector<uint8_t> HashData(const std::vector<uint8_t>& data);
};
#endif /* defined(__EccTool__NativeCrypto__) */
