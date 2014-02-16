//
//  EccAlg.cpp
//  EccTool
//
//  Created by Josh Strom on 1/12/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#include "EccAlg.h"
#include "Point.h"
#include "NativeCrypto.h"
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

vector<uint8_t> Xor(const vector<uint8_t>& lhs, const vector<uint8_t>& rhs)
{
    if(lhs.size() != rhs.size())
        throw invalid_argument("Both vectors must be the same size for XOR operation.");
    
    vector<uint8_t> result(lhs.size());
    for(unsigned int i = 0; i < lhs.size(); i++)
        result[i] = lhs[i] ^ rhs[i];
    
    return result;
}

vector<uint8_t> EccAlg::Encrypt(const vector<uint8_t> plaintext)
{
    // Compute a shared secret from which to derive a symmetric key.
    // First, choose a random number r such that 0 < r < n (the curve
    // base point order).
    
    // Next, derive a shared secret S by multiplying r by the recipient's
    // public key.
    
    // Lastly calculate the point R by multiplying the base point with r.
    // R is a "tag" value that will allow the recipient to derive the
    // shared secret.
    
    BigInteger r = GenerateRandomPositiveIntegerLessThan(_curve.GetBasePointOrder());
    Point S = _curve.MultiplyPointOnCurveWithScalar(_curve.GetBasePoint(), r);
    auto R = _curve.MultiplyPointOnCurveWithScalar(_publicKey, r).Serialize(); // We only need this serialized.
    
    // Use the shared secret S to derive a key. Note: normally, some additional
    // shared information would be used as the "salt" value here. However, in this
    // example, we will use the 'x' value of the point as the password and the 'y'
    // value of the point as the salt. The length of the derived key will be  the
    // length of the plaintext.
    vector<uint8_t> encryptionKey = NativeCrypto::DeriveKey(S.x.GetRawInteger().GetMagnitudeBytes(), S.y.GetRawInteger().GetMagnitudeBytes(), plaintext.size());
    
    // Encrypt with XOR encryption.
    auto ciphertext = Xor(plaintext, encryptionKey);
    
    // The output of this function is the ciphertext and the tag with the following format:
    // R || ciphertext (where '||' denotes concatenation)
    vector<uint8_t> output(R.size() + ciphertext.size());
    auto outIterator = output.begin();
    
    copy(R.begin(), R.end(), outIterator);
    outIterator += R.size();
    copy(ciphertext.begin(), ciphertext.end(), outIterator);
    
    return output;
}
















