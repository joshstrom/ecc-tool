
// Ensure correct platform.
#ifndef __APPLE__ // Mac
#error "Attempting to compile Macintosh sources on non-Macintosh platform."
#endif

#include <CommonCrypto/CommonCryptor.h>
#include <CommonCrypto/CommonKeyDerivation.h>
#include <CommonCrypto/CommonDigest.h>

#include <vector>
#include <stdint.h>

#include "NativeCrypto.h"

using namespace std;

// Derive a key of provided size with password and salt.
vector<uint8_t> NativeCrypto::DeriveKey(const vector<uint8_t>& password, const vector<uint8_t>& salt, size_t keySize)
{
	// For Apple, use ComonCrypto pkbdf2 implementation.
    vector<uint8_t> derivedKey(keySize);    
    CCKeyDerivationPBKDF(kCCPBKDF2, reinterpret_cast<const char*>(password.data()), password.size(), salt.data(), salt.size(), kCCPRFHmacAlgSHA256, 10000, derivedKey.data(), derivedKey.size());
        
    return derivedKey;
}

// Hash the provided data with the SHA 256 hash algorithm.
vector<uint8_t> NativeCrypto::HashData(const vector<uint8_t>& data)
{
	// For Apple, use the CommonCrypto implementation of SHA 256.
    vector<uint8_t> digest(CC_SHA256_DIGEST_LENGTH);
    CC_SHA256(data.data(), static_cast<CC_LONG>(data.size()), digest.data());
        
    return digest;
}