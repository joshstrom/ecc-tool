//
//  NativeCrypto.h
//  EccTool
//
//  Created by Josh Strom on 2/13/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#ifndef __EccTool__NativeCrypto__
#define __EccTool__NativeCrypto__

// This class provides an interface into system dependant crypto primitives.
// Compile-time preprocessing is used to select appropriate code for the operating system.

#include <iostream>
#include <vector>

using namespace std;

#ifdef __APPLE__
#include <CommonCrypto/CommonCryptor.h>
#include <CommonCrypto/CommonKeyDerivation.h>
#endif

class NativeCrypto
{
    
public:
    
    // Derive a key of provided size with password and salt.
    static vector<uint8_t> DeriveKey(const vector<uint8_t>& password, const vector<uint8_t>& salt, size_t keySize)
    {
#ifdef __APPLE__
        
        // For Apple, use ComonCrypto pkbdf2 implementation.
        vector<uint8_t> derivedKey(keySize);
        
        CCKeyDerivationPBKDF(kCCPBKDF2, reinterpret_cast<const char*>(password.data()), password.size(), salt.data(), salt.size(), kCCPRFHmacAlgSHA256, 10000, derivedKey.data(), derivedKey.size());
        
        return derivedKey;
        
#else // Windows TODO: insert win32 #ifdef here, don't just use #else
        
        // TODO: Implement for windows.
        return vector<uint8_t>(keySize);
        
#endif
    }
    
};
#endif /* defined(__EccTool__NativeCrypto__) */
