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
#include <memory>

using namespace std;

#ifdef __APPLE__ // Mac
#include <CommonCrypto/CommonCryptor.h>
#include <CommonCrypto/CommonKeyDerivation.h>
#include <CommonCrypto/CommonDigest.h>
#elif _MSC_VER // Windows
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include "bcrypt.h"
#else // Other (unsupported)
#error "Unsupported Platform."
#endif

class NativeCrypto
{
    
public:
    
    // Derive a key of provided size with password and salt.
    static vector<uint8_t> DeriveKey(const vector<uint8_t>& password, const vector<uint8_t>& salt, size_t keySize)
    {
#ifdef __APPLE__ // Mac
        
        // For Apple, use ComonCrypto pkbdf2 implementation.
        vector<uint8_t> derivedKey(keySize);
        
        CCKeyDerivationPBKDF(kCCPBKDF2, reinterpret_cast<const char*>(password.data()), password.size(), salt.data(), salt.size(), kCCPRFHmacAlgSHA256, 10000, derivedKey.data(), derivedKey.size());
        
        return derivedKey;
        
#elif _MSC_VER // Windows
        
        // For Windows, use the CNG (Crypto Next Gen) BCrypt APIs for key derivation.
        
		// Deleter for alg handle to ensure deterministic close.
		auto deleter = [] (BCRYPT_ALG_HANDLE* handle) 
		{ 
			if (handle == nullptr)
				return;

			BCryptCloseAlgorithmProvider(*handle, 0); 
			delete handle; 
		};
		unique_ptr<BCRYPT_ALG_HANDLE, decltype(deleter)> prfAlgHandle(new BCRYPT_ALG_HANDLE(), deleter);

        // Open the algorithm provider (SHA-256) for the as the PRF for the key derivation function.
		NTSTATUS status = BCryptOpenAlgorithmProvider(prfAlgHandle.get(), 
													  BCRYPT_SHA256_ALGORITHM, 
													  MS_PRIMITIVE_PROVIDER, 
													  BCRYPT_ALG_HANDLE_HMAC_FLAG);
		if (status != 0)
			throw runtime_error("Unable to create hash alg for key derivation.");
		
        // Use the Password Based Key Derivation Function version 2 (PBKDF2) algorithm to derive a key.
        // Note: we use const_cast on the the password and salt buffers because although these buffers are
        // unchanged by the function, they are not declared as const in the function declaration.
		vector<uint8_t> derivedKey(keySize);
		status = BCryptDeriveKeyPBKDF2(*prfAlgHandle,
									   const_cast<uint8_t*>(password.data()),
									   password.size(),
									   const_cast<uint8_t*>(salt.data()),
									   salt.size(),
									   10000,
									   derivedKey.data(),
									   derivedKey.size(),
									   0);
		if (status != 0)
			throw runtime_error("Unable to derive key.");

		return derivedKey;
        
#else
#error "Unsupported platform."
#endif
    }
    
    // Hash the provided data with the SHA 256 hash algorithm.
    static vector<uint8_t> HashData(const vector<uint8_t>& data)
    {
#ifdef __APPLE__
        
        // For Apple, use the CommonCrypto implementation of SHA 256.
        vector<uint8_t> digest(CC_SHA256_DIGEST_LENGTH);
        CC_SHA256(data.data(), static_cast<CC_LONG>(data.size()), digest.data());
        
        return digest;
        
#elif _MSC_VER
#error "Hash not implemented."
#else
#error "Unsupported Platform."
#endif
    }
};
#endif /* defined(__EccTool__NativeCrypto__) */
