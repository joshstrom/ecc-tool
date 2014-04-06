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
#include <stdio.h>
#include <vector>
#include "DefinedCurveDomainParameters.h"
#include "EccDefs.h"

using namespace std;
using namespace ecc;

// The following are a few sets of standard curve parameters.
// Curve definition found here: http://www.secg.org/collateral/sec2_final.pdf

const vector<string> ecc::GetSupportedCurves()
{
    vector<string> curves;
    
    // Add all supported curves by name.
    curves.push_back("secp112r1");
    curves.push_back("secp256k1");
    
    return curves;
}

const DomainParameters ecc::GetCurveByName(const string& name)
{
    if(name == "secp256k1")
        return GetSecp256k1Curve();
    if(name == "secp112r1")
        return GetSecp112r1Curve();
    
    throw invalid_argument("Unsupported curve: " + name);
}

DomainParameters ecc::GetSecp256k1Curve()
{
    DomainParameters params = {
        "secp256k1",
        "FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE FFFFFC2F", //p
        "00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000", //a
        "00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000007", //b
        "04 79BE667E F9DCBBAC 55A06295 CE870B07 029BFCDB 2DCE28D9 59F2815B 16F81798 483ADA77 26A3C465 5DA4FBFC 0E1108A8 FD17B448 A6855419 9C47D08F FB10D4B8", //G (uncompressed)
        "FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE BAAEDCE6 AF48A03B BFD25E8C D0364141", //n
        "01" //h
    };
    
    return params;
}

DomainParameters ecc::GetSecp112r1Curve()
{
    DomainParameters params = {
        "secp112r1", //name
        "DB7C 2ABF62E3 5E668076 BEAD208B", //p
        "DB7C 2ABF62E3 5E668076 BEAD2088", //a
        "659E F8BA0439 16EEDE89 11702B22", //b
        "04 09487239 995A5EE7 6B55F9C2 F098A89C E5AF8724 C0A23E0E 0FF77500", //G (uncompressed)
        "DB7C 2ABF62E3 5E7628DF AC6561C5", //n
        "01" //h
    };
    
    return params;
}
