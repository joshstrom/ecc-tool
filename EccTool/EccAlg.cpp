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
#include <cassert>

EccAlg::EccAlg(const EllipticCurve& curve) : _curve(curve)
{
    srand(static_cast<unsigned int>(time(nullptr)));
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

bool EccAlg::HasPrivateKey() const
{
    return _hasPrivateKey;
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

vector<uint8_t> EccAlg::Encrypt(const vector<uint8_t>& plaintext) const
{
    // The following process is derived from the SEC 1: Elliptic Curve Cryptography spec
    //  found here: http://www.secg.org/collateral/sec1_final.pdf (Section 5.1.3)
    
    // Compute a shared secret from which to derive a symmetric key.
    // First, choose a random number r such that 0 < r < n (the curve
    // base point order).
    
    // Next, derive a shared secret S by multiplying r by the recipient's
    // public key.
    
    // Lastly calculate the point R by multiplying the base point with r.
    // R is a "tag" value that will allow the recipient to derive the
    // shared secret.
    
    BigInteger r = GenerateRandomPositiveIntegerLessThan(_curve.GetBasePointOrder());
    Point S = _curve.MultiplyPointOnCurveWithScalar(_publicKey, r);
    auto R = _curve.MultiplyPointOnCurveWithScalar(_curve.GetBasePoint(), r).Serialize(); // We only need this serialized.
    
    // Use the shared secret S to derive a key. Note: normally, some additional
    // shared information would be used as the "salt" value here. However, in this
    // example, we will use the 'x' value of the point as the password and the 'y'
    // value of the point as the salt. The length of the derived key will be  the
    // length of the plaintext.
    vector<uint8_t> encryptionKey = NativeCrypto::DeriveKey(S.x.GetRawInteger().GetMagnitudeBytes(), S.y.GetRawInteger().GetMagnitudeBytes(), plaintext.size());
    
    // Encrypt with XOR encryption.
    auto encryptedMessage = Xor(plaintext, encryptionKey);
    
    // The output of this function is the ciphertext and the tag with the following format:
    // R || encryptedMessage (where '||' denotes concatenation).
    // Note: The actual specification defines that a MAC should be computed over the encryptedMessage,
    //  and appended to the output following the encryptedMessage. This is ommited for simplicity in this
    //  implementation.
    vector<uint8_t> ciphertext(R.size() + encryptedMessage.size());
    auto outIterator = ciphertext.begin();
    
    copy(R.begin(), R.end(), outIterator);
    outIterator += R.size();
    copy(encryptedMessage.begin(), encryptedMessage.end(), outIterator);
    
    return ciphertext;
}

vector<uint8_t> EccAlg::Decrypt(const vector<uint8_t>& ciphertext) const
{
    EnsurePrivateKeyAvailable();
    // The following process is derived from the SEC 1: Elliptic Curve Cryptography spec
    //  found here: http://www.secg.org/collateral/sec1_final.pdf (Section 5.1.4)
    
    // The ciphertext will contain R (a tag value which allows us to derive the shared secret) and
    //  the ciphertext itself.
    // Note: the specification also defines a MAC value, but for simplicity of this example, the MAC
    //  value is ignored.
    // The format of the buffer is as follows: R || ciphertext (where '||' denotes concatenation).
    
    // First, parse out the point. It is encoded according to the curve, and thus can be parsed.
    // The Parse routine ignores any additional data appended after the point.
    Point R = _curve.MakePointOnCurve(ciphertext);
    
    // The encrypted message is the remaining portion of the buffer that does not contian the point.
    vector<uint8_t> encryptedMessage(ciphertext.begin() + R.ComputeUncompressedSize(), ciphertext.end());
    
    // Compute the shared secret S by multiplying R with the private key.
    //  This works because:
    // S = pubKey * r (where S is the shared scret we are trying to derive)
    // R = G * r (where r is the generated secret integer)
    // pubKey = G * privKey
    // Therefore S = pubKey * r
    //             = (G * privKey) * r  [by substitution of pubKey]
    //             = (G * r) * privKey  [by rearanging the mulptilcands)
    //             = R * privKey        [by substitution of G * r]
    Point S = _curve.MultiplyPointOnCurveWithScalar(R, _privateKey);
    
    // Use the shared secret S to derive a key. Note: normally, some additional
    // shared information would be used as the "salt" value here. However, in this
    // example, we will use the 'x' value of the point as the password and the 'y'
    // value of the point as the salt. The length of the derived key will be  the
    // length of the encrypted message.
    auto encryptionKey = NativeCrypto::DeriveKey(S.x.GetRawInteger().GetMagnitudeBytes(), S.y.GetRawInteger().GetMagnitudeBytes(), encryptedMessage.size());
    
    // Decrypt with XOR encryption.
    auto plaintext = Xor(encryptedMessage, encryptionKey);
    
    return plaintext;
}

vector<uint8_t> EccAlg::Sign(const vector<uint8_t>& message) const
{
    EnsurePrivateKeyAvailable();
    // Compute a hash of the message and select the left-most n bits,
    // where n is the bitlength of the curve order. Store these bits
    // in the integer z.
    auto hash = NativeCrypto::HashData(message);
    
    // Select the bits by determining how many bits must must be removed
    // and shifting the integer z right to remove the right-most bits.
    BigInteger z(hash);
    size_t Ln = min(_curve.GetBasePointOrder().GetBitSize(), z.GetBitSize());
    unsigned int bitsToRemove = static_cast<unsigned int>(z.GetBitSize() - Ln);
    z >>= bitsToRemove;
    
    // Generate an ephemeral keypair, k (private key) and Pk (public key).
    auto k = GenerateRandomPositiveIntegerLessThan(_curve.GetBasePointOrder());
    auto R = _curve.MultiplyPointOnCurveWithScalar(_curve.GetBasePoint(), k);
    
    // Calculate an integer r by taking the x-value of the previously generated
    // point mod the base point order. If zero, generate a new k and start again.
    auto n = make_shared<BigInteger>(_curve.GetBasePointOrder());
    auto r = FieldElement::MakeElement(R.x, n);//FieldElement(Pk.x.GetRawInteger() % *n, n);
    assert(r != 0);
    
    // Calculate an integer s by adding z to the multiplication of the private key with s,
    // then dividing this by the ephemral private key, mod n.
    auto fieldElementK = FieldElement::MakeElement(k, n);
    auto s = ((FieldElement::MakeElement(z, n) + FieldElement::MakeElement(_privateKey, n) * r)) / fieldElementK;
    assert(s != 0);
    
    auto signature = Point(r, s);
    return signature.Serialize();
}

bool EccAlg::Verify(const vector<uint8_t>& message, const vector<uint8_t>& signature) const
{
    // The point is in the field of the curve's base point order domain parameter.
    auto n = make_shared<BigInteger>(_curve.GetBasePointOrder());
    
    // The signature is encoded as a point. Parse it (catching any exceptions).
    Point signaturePoint;
    try
    {
        signaturePoint = Point::Parse(signature, 0, n);
    }
    catch(exception& ex)
    {
        // We only want to output a message here in in debug mode.
        utilities::DebugLog(string("Error parsing signature point: ").append(ex.what()));
        return false;
    }

    // The signature values r and s are encoded as the x and y coordinates of
    // the signature point.
    auto& r = signaturePoint.x;
    auto& s = signaturePoint.y;
    
    // They both must be in the range of (0,n) (exclusive);
    if(r.GetRawInteger() <= 0 || r.GetRawInteger() >= *n)
    {
        // We only want to output a message here in in debug mode.
        utilities::DebugLog("Signature invalid - r out of range.");
        return false;
    }
    
    if(s.GetRawInteger() <= 0 || s.GetRawInteger() >= *n)
    {
        // We only want to output a message here in in debug mode.
        utilities::DebugLog("Signature invalid - s out of range.");
        return false;
    }
    
    // Compute a hash of the message and select the left-most n bits,
    // where n is the bitlength of the curve order. Store these bits
    // in the integer z.
    auto hash = NativeCrypto::HashData(message);
    
    // Select the bits by determining how many bits must must be removed
    // and shifting the integer z right to remove the right-most bits.
    BigInteger z(hash);
    size_t Ln = min(_curve.GetBasePointOrder().GetBitSize(), z.GetBitSize());
    unsigned int bitsToRemove = static_cast<unsigned int>(z.GetBitSize() - Ln);
    z >>= bitsToRemove;

    // Let w be the multiplicative inverse of s in the mod field n.
    auto w = s.GetInverse();
    
    // Let u1 be the multiplicatin of z with w (mod n) and u2 be the multiplication
    // of r with w (mod n).
    auto u1 = FieldElement(z, n) * w;
    auto u2 = r * w;
    
    // Calculate the check point by adding the multiplication of u1 and the curve generator point
    // to the multiplication of u2 and the alg's public key.
    auto firstAddend = _curve.MultiplyPointOnCurveWithScalar(_curve.GetBasePoint(), u1.GetRawInteger());
    auto secondAddend = _curve.MultiplyPointOnCurveWithScalar(_publicKey, u2.GetRawInteger());
    auto checkPoint = _curve.AddPointsOnCurve(firstAddend, secondAddend);
    
    // The signature is valid if r is equivalent to checkPoint:x (mod n).
    return r == (checkPoint.x.GetRawInteger() % *n);
}













