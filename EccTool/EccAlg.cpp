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

EccAlg::EccAlg(const EllipticCurve& curve) : _curve(curve)
{
}

void EccAlg::GenerateKeys()
{
    // Generate a random private key appropriate for this curve.
    _privateKey = GenerateRandomPositiveIntegerLessThan(_curve.GetBasePointOrder());
    
    // Generate the public key point matching the private key starting from the base point.
    const Point& basePoint = _curve.GetBasePoint();
    _publicKey = _curve.MultiplyPointOnCurveWithScalar(basePoint, _privateKey);
    
    // Validate public key point.
    if(!_curve.CheckPointOnCurve(_publicKey))
        throw runtime_error("Generated public key not on curve.");
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

void EccAlg::LoadKeys(const string& archivedKeys)
{
    // Formate of keys: [<private>:<publicX>:<publicY>]
    if(archivedKeys[0] != '[' || archivedKeys[archivedKeys.size() - 1] != ']')
        throw invalid_argument("Not valid format for serialized keys. Missing outer bracket delimiters.");
    
    string stringToParse(archivedKeys.begin() + 1, archivedKeys.begin() + archivedKeys.size() - 1);
    string delimiter = ":";
    
    // Parse the private key element from the archived string.
    size_t elementDelimiter = stringToParse.find(delimiter);
    if(elementDelimiter == string::npos)
        throw invalid_argument("Not valid format for serialized keys. Only first element found.");
    BigInteger parsedPrivateKey = (stringToParse.substr(0, elementDelimiter));
    stringToParse.erase(0, elementDelimiter + 1);
    
    // Point to hold the public key components.
    Point parsedPublicKey;
    
    // Parse the PublicX key element from the archived string.
    elementDelimiter = stringToParse.find(delimiter);
    if(elementDelimiter == string::npos)
        throw invalid_argument("Not valid format for serialized keys. Third element not in string.");
    parsedPublicKey.x = BigInteger(stringToParse.substr(0, elementDelimiter));
    stringToParse.erase(0, elementDelimiter + 1);
    
    // Parse the PublicY key element from the archived string.
    elementDelimiter = stringToParse.find(delimiter);
    if(elementDelimiter != string::npos)
        throw invalid_argument("Not valid format for serialized keys. String not ending when expected.");
    parsedPublicKey.y = BigInteger(stringToParse.substr(0, elementDelimiter));
    
    // Save the key components.
    swap(_privateKey, parsedPrivateKey);
    swap(_publicKey, parsedPublicKey);
    
    // Test to ensure that the public key is on the curve.
    if(!_curve.CheckPointOnCurve(_publicKey))
        throw invalid_argument("Invalid key for curve.");
}

const string EccAlg::SaveKeys() const
{
    // Order of keys: [<private>:<publicX>:<publicY>]
    stringstream ss;
    ss << '[' << _privateKey << ':' << _publicKey.x << ':' << _publicKey.y << ']';
    
    return ss.str();
}

const string EccAlg::KeysToString(bool includePrivate) const
{
    stringstream ss;
    ss << "Public: " << _publicKey;
    
    if(includePrivate)
        ss << endl << "Private: " << _privateKey;
    
    return ss.str();
}
