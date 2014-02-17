//
//  EccAlg.h
//  EccTool
//
//  Created by Josh Strom on 1/12/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
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
    
    vector<uint8_t> Encrypt(const vector<uint8_t>& plaintext);
    
    vector<uint8_t> Decrypt(const vector<uint8_t>& ciphertext);
};

class no_private_key final : public runtime_error
{
public:
    no_private_key() : runtime_error("Cannot peform private key operation. No private key set.")
    {
    }
};


#endif /* defined(__EccTool__EccAlg__) */
