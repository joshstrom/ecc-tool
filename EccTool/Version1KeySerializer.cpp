//
//  Version1KeySerializer.cpp
//  EccTool
//
//  Created by Josh Strom on 2/8/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#include "Version1KeySerializer.h"
#include "Utilities.h"
#include "EccAlg.h"
#include "DefinedCurveDomainParameters.h"
#include "EllipticCurve.h"

#include <sstream>

string Version1KeySerializer::SerializePublicKeys(const EccAlg& alg) const
{
    // Save format: <curve name>:[<public>]
    vector<uint8_t> publicKey = alg.GetPublicKey();
    
    stringstream ss;
    ss << alg.GetCurveName() << ":[" << publicKey << ']';
    
    return ss.str();
}

string Version1KeySerializer::SerializePrivateKeys(const EccAlg& alg) const
{
    // Save format: <curve name>:[<public>:<private>]
    vector<uint8_t> publicKey = alg.GetPublicKey();
    vector<uint8_t> privateKey = alg.GetPrivateKey();
    
    stringstream ss;
    ss << alg.GetCurveName() << ":[" << publicKey << ':' << privateKey << ']';
    
    return ss.str();
}

EccAlg Version1KeySerializer::ParseKeys(const string& keys) const
{
    // Save format: <curve name>:[<private>:<public>]
    
    // First, get the curve name and use it to create the curve.
    unsigned long startingIndex = 0;
    static const char delimiter = ':';
    size_t elementDelimiter = keys.find(delimiter, startingIndex);
    if(elementDelimiter == string::npos)
        throw invalid_argument("Invalid file format for Version1 Key Storage.");
    
    string curveName = keys.substr(startingIndex, elementDelimiter);
    startingIndex = elementDelimiter + 1;

    EllipticCurve curve(ecc::GetCurveByName(curveName));
    EccAlg createdAlg(curve);
    
    // Formate of keys: [<private>:<publicX>:<publicY>]
    if(keys[startingIndex] != '[' || keys[keys.size() - 1] != ']')
        throw invalid_argument("Not valid format for serialized keys. Missing outer bracket delimiters.");
    
    // Skip the '['
    startingIndex++;
    
    // Parse the public key element from the archived string.
    elementDelimiter = keys.find(delimiter, startingIndex);
    string temp;
    if(elementDelimiter == string::npos)
    {
        // Only public key here.
        // Ignore the ending ']'
        vector<uint8_t> publicKey(utilities::HexStringToBytes(keys.substr(startingIndex, keys.size() - startingIndex - 1)));
        createdAlg.SetKey(publicKey);
        return createdAlg;
    }

    // Otherwise, parse the private key as well.
    vector<uint8_t> publicKey(utilities::HexStringToBytes(keys.substr(startingIndex, elementDelimiter - startingIndex)));
    startingIndex = elementDelimiter + 1;
    
    // Check for end of data (delimited by a ']'), if this is the end, there is only a public key
    // stored here.
    if(keys[startingIndex] == ']')
    {
        createdAlg.SetKey(publicKey);
        return createdAlg;
    }
    
    // Parse the private key element from the archived string.
    elementDelimiter = keys.find(delimiter, startingIndex);
    if(elementDelimiter != string::npos)
        throw invalid_argument("Not valid format for serialized keys. String not ending when expected.");
    temp = keys.substr(startingIndex, keys.size() - startingIndex - 1);
    vector<uint8_t> privateKey(utilities::HexStringToBytes(temp));

    createdAlg.SetKey(publicKey, privateKey);
    
    return createdAlg;
}















