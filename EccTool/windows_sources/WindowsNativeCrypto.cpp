
// Ensure correct platform.
#ifndef _MSC_VER // Mac
#error "Attempting to compile Windows sources on non-Windows platform."
#endif

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include "bcrypt.h"

#include <vector>
#include <stdint.h>

#include "NativeCrypto.h"

using namespace std;

// Deleter and typedef to determinstically close the algorithm provider.
struct AlgHandleDeleter
{
	void operator()(BCRYPT_ALG_HANDLE* handle)
	{
		if (handle == nullptr)
			return;

		BCryptCloseAlgorithmProvider(*handle, 0);
		delete handle;
	}
};
typedef unique_ptr<BCRYPT_ALG_HANDLE, AlgHandleDeleter> AlgHandle;

// Deleter and typedef to deterministically close the hash handle.
struct HashHandleDeleter 
{
	void operator()(BCRYPT_HASH_HANDLE* handle)
	{
		if (handle == nullptr)
			return;

		BCryptDestroyHash(*handle);
		delete handle;
	}
};
typedef unique_ptr<BCRYPT_HASH_HANDLE, HashHandleDeleter> HashHandle;

// Derive a key of provided size with password and salt.
vector<uint8_t> NativeCrypto::DeriveKey(const vector<uint8_t>& password, const vector<uint8_t>& salt, size_t keySize)
{
	// For Windows, use the CNG (Crypto Next Gen) BCrypt APIs for key derivation.

	// Open the algorithm provider (SHA-256) for the as the PRF for the key derivation function.
	AlgHandle prfAlgHandle(new BCRYPT_ALG_HANDLE());
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
}

// Hash the provided data with the SHA 256 hash algorithm.
vector<uint8_t> NativeCrypto::HashData(const vector<uint8_t>& data)
{
	// For Windows, use the CNG (Crypto Next Gen) BCrypt APIs for hashing.

	// Open the algorithm provider (SHA-256) for the as the PRF for the key derivation function.
	AlgHandle hashAlgHandle(new BCRYPT_ALG_HANDLE());
	NTSTATUS status = BCryptOpenAlgorithmProvider(hashAlgHandle.get(),
		BCRYPT_SHA256_ALGORITHM,
		MS_PRIMITIVE_PROVIDER,
		0);
	if (status != 0)
		throw runtime_error("Unable to create hash alg.");

	HashHandle hashHandle(new BCRYPT_HASH_HANDLE());
	status = BCryptCreateHash(*hashAlgHandle, hashHandle.get(), NULL, 0, NULL, 0, 0);

	vector<uint8_t> hash(32);
	status = BCryptHashData(*hashHandle, const_cast<uint8_t*>(data.data()), data.size(), 0);
	if (status != 0)
		throw runtime_error("Unable to hash data.");
	status = BCryptFinishHash(*hashHandle, const_cast<uint8_t*>(hash.data()), hash.size(), 0);
	if (status != 0)
		throw runtime_error("Unable to finish hash.");

	return hash;
}