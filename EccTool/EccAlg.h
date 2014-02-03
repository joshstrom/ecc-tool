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
    
    // Generates a random positive integer in the range 0 < gnerated < max.
    static BigInteger GenerateRandomPositiveIntegerLessThan(const BigInteger& max);
    
public:
    // Creates an Elliptic Curve Cryptography alg with the given curve.
    EccAlg(const EllipticCurve& curve);
    
    // Generates random keys for the given curve.
    void GenerateKeys();
    
    // Loads stored keys from a string.
    void LoadKeys(const string& archivedKeys);
    
    // Persists the keys pair in the returned string.
    const string SaveKeys() const;
    
    // Creates a string with a printable version of the string.
    //  Optionally includes private key.
    const string KeysToString(bool includePrivate) const;
    
    //vector<uint8_t> Encrypt(const vector<uint8_t> plaintext);
    //vector<uint8_t> Decrypt(const vector<uint8_t> ciphertext);
};


#endif /* defined(__EccTool__EccAlg__) */
