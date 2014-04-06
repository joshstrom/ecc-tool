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
#include "KeySerializer.h"
#include "Utilities.h"
#include "EccAlg.h"
#include "DefinedCurveDomainParameters.h"
#include "EllipticCurve.h"

#include <sstream>

string KeySerializer::SerializePublicKeys(const EccAlg& alg) const
{
    // Save format: <curve name>:[<public>]
    vector<uint8_t> publicKey = alg.GetPublicKey();
    
    stringstream ss;
    ss << alg.GetCurveName() << ":[" << publicKey << ']';
    
    return ss.str();
}

string KeySerializer::SerializePrivateKeys(const EccAlg& alg) const
{
    // Save format: <curve name>:[<public>:<private>]
    vector<uint8_t> publicKey = alg.GetPublicKey();
    vector<uint8_t> privateKey = alg.GetPrivateKey();
    
    stringstream ss;
    ss << alg.GetCurveName() << ":[" << publicKey << ':' << privateKey << ']';
    
    return ss.str();
}

EccAlg KeySerializer::ParseKeys(const string& keys) const
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















