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
        
#elif _MSC_VER // Windows TODO: insert win32 #ifdef here, don't just use #else
        
		// Deleter for alg handle to ensure deterministic close.
		auto deleter = [] (BCRYPT_ALG_HANDLE* handle) 
		{ 
			if (handle == nullptr)
				return;

			BCryptCloseAlgorithmProvider(*handle, 0); 
			delete handle; 
		};

		unique_ptr<BCRYPT_ALG_HANDLE, decltype(deleter)> prfAlgHandle(new BCRYPT_ALG_HANDLE(), deleter);

		NTSTATUS status = BCryptOpenAlgorithmProvider(prfAlgHandle.get(), 
													  BCRYPT_SHA256_ALGORITHM, 
													  MS_PRIMITIVE_PROVIDER, 
													  BCRYPT_ALG_HANDLE_HMAC_FLAG);
		if (status != 0)
			throw runtime_error("Unable to create hash alg for key derivation.");
		
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
    
};
#endif /* defined(__EccTool__NativeCrypto__) */
