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
#ifndef __EccTool__EccAlg__
#define __EccTool__EccAlg__

#include <iostream>
#include <tuple>
#include "EllipticCurve.h"
#include "BigInteger.h"

using namespace std;

class EccAlg
{
private:
    // The curve used by this alg.
    EllipticCurve _curve;
    
    // This curve private key.
    BigInteger _privateKey;
    
    // The public key point on this curve.
    Point _publicKey;
    
    // Indicates whether the alg is set up for private key operations.
    bool _hasPrivateKey;
    
    // Generates a random positive integer in the range 0 < generated < max.
    static BigInteger GenerateRandomPositiveIntegerLessThan(const BigInteger& max);
    
    // Throws and exception if the private key is not available.
    void EnsurePrivateKeyAvailable() const;
    
public:
    // Creates an Elliptic Curve Cryptography alg with the given curve.
    EccAlg(const EllipticCurve& curve);
    
    // Generates random keys for the given curve.
    void GenerateKeys();
    
    // Creates a string with a printable version of the string.
    //  Optionally includes private key.
    const string KeysToString(bool includePrivate) const;
    
    // Sets the public and private keys for this instance. Replaces any previously
    //  set or generted keys for this instance.
    // - pubicKey is the public key as a serialized point.
    // - privateKey is the the binary representation of the private key.
    void SetKey(const vector<uint8_t> publicKey, const vector<uint8_t> privateKey);
    
    // Sets the public key for this instance. Replaces any previously set or
    //  generated keys for this instance. Note that private key operations cannot
    //  be performed if only the public key has been set.
    // - publicKey is the public key as a serialized point.
    void SetKey(const vector<uint8_t> publicKey);
    
    // Gets the public key for this alg as a serilized point. Note: this call
    //  will throw if the priate key was not previously either set or generated.
    const vector<uint8_t> GetPublicKey() const;
    
    // Gets the private key for this alg as a binary integer representation. Note: this call
    //  will throw an exception if the priate key was not previously either set or generated.
    const vector<uint8_t> GetPrivateKey() const;
    
    // Gets the name of the curve used to back this algorithm.
    string GetCurveName() const;
    
    // Encrypts the given plaintext (uses public key).
    vector<uint8_t> Encrypt(const vector<uint8_t>& plaintext) const;
    
    // Decrypts the given ciphertext (uses private key).
    vector<uint8_t> Decrypt(const vector<uint8_t>& ciphertext) const;
    
    // Signs the given message with the alg's privte key.
    vector<uint8_t> Sign(const vector<uint8_t>& message) const;
    
    // Verifies the given signed message with the alg's public key.
    bool Verify(const vector<uint8_t>& message, const vector<uint8_t>& signature) const;
    
    // Returns wether this instance has a private key or was loaded from a public key.
    bool HasPrivateKey() const;
};

// The exception thrown if the private key is not set for an option which requires it.
class no_private_key final : public runtime_error
{
public:
    no_private_key() : runtime_error("Cannot peform private key operation. No private key set.")
    {
    }
};


#endif /* defined(__EccTool__EccAlg__) */
