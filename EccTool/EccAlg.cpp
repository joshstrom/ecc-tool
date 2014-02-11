//
//  EccAlg.cpp
//  EccTool
//
//  Created by Josh Strom on 1/12/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#include "EccAlg.h"
#include "Point.h"
#include <sstream>
#include <ctime>

EccAlg::EccAlg(const EllipticCurve& curve) : _curve(curve)
{
}

void EccAlg::GenerateKeys()
{
    // Generate a random private key appropriate for this curve.
    BigInteger privateKey = GenerateRandomPositiveIntegerLessThan(_curve.GetBasePointOrder());
    
    // Generate the public key point matching the private key starting from the base point.
    const Point& basePoint = _curve.GetBasePoint();
    Point publicKey = _curve.MultiplyPointOnCurveWithScalar(basePoint, privateKey);
    
    // Validate public key point.
    if(!_curve.CheckPointOnCurve(publicKey))
        throw runtime_error("Generated public key not on curve.");
    
    // All successful, set the public and private keys.
    swap(_publicKey, publicKey);
    swap(_privateKey, privateKey);
    _hasPrivateKey = true;
}

BigInteger EccAlg::GenerateRandomPositiveIntegerLessThan(const BigInteger& max)
{
    // Determine the maximum number of random bits we need.
    size_t maxRandomBitsNeeded = max.GetMostSignificantBitIndex();
    
    BigInteger k; // This will be the private key.
    
    srand(static_cast<unsigned int>(time(nullptr)));
    for(size_t i = 0; i <= maxRandomBitsNeeded; i++)
    {
        k <<= 1;
        if((rand() % 2) != 0)
            k.SetBitAt(0);
    }
    
    // Ensure that the resulting random integer k < n.
    //  If k >= n, unset the MSB, this should result in an integer of the appropriate size.
    if(k >= max)
        k.ClearBitAt(k.GetMostSignificantBitIndex());
    
    if(k >= max || k <= 0)
    {
        stringstream ss;
        ss << "Number in invalid range generated. Required: < " << max << ", Actual: " << k;
        throw runtime_error(ss.str());
    }
    
    return k;
}

void EccAlg::SetKey(const vector<uint8_t> publicKey, const vector<uint8_t> privateKey)
{
    // Make both key values usable.
    Point publicKeyPoint = _curve.MakePointOnCurve(publicKey);
    BigInteger privateKeyValue = BigInteger(privateKey);
    
    // Validate key-pair to ensure that PubKey == G*PrivKey
    if(_curve.MultiplyPointOnCurveWithScalar(_curve.GetBasePoint(), privateKeyValue) != publicKeyPoint)
        throw invalid_argument("Pub/Priv key-pair invalid.");
    
    // Set the keys in the algorithm.
    swap(_publicKey, publicKeyPoint);
    swap(_privateKey, privateKeyValue);
    _hasPrivateKey = true;
}

void EccAlg::SetKey(const vector<uint8_t> publicKey)
{
    // Make the public key value usable.
    Point publicKeyPoint = _curve.MakePointOnCurve(publicKey);
    
    swap(_publicKey, publicKeyPoint);
    _privateKey = 0;
    _hasPrivateKey = false;
}

const vector<uint8_t> EccAlg::GetPublicKey() const
{
    return _publicKey.Serialize();
}

const vector<uint8_t> EccAlg::GetPrivateKey() const
{
    EnsurePrivateKeyAvailable();
    return _privateKey.GetMagnitudeBytes();
}

const string EccAlg::KeysToString(bool includePrivate) const
{
    stringstream ss;
    ss << "Public: " << _publicKey.Serialize();
    
    if(includePrivate)
        ss << endl << "Private: " << _privateKey;
    
    return ss.str();
}

string EccAlg::GetCurveName() const
{
    return _curve.GetCurveName();
}

void EccAlg::EnsurePrivateKeyAvailable() const
{
    if(!_hasPrivateKey)
        throw no_private_key();
}




