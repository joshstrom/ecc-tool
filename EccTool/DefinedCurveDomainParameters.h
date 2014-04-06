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
#ifndef EccTool_DefinedCurveDomainParameters_h
#define EccTool_DefinedCurveDomainParameters_h

#include "EccDefs.h"
#include <exception>

namespace ecc
{
    // Returns a list of supported curve names.
    const vector<string> GetSupportedCurves();
    
    // Returns the specified curve parameters.
    const DomainParameters GetCurveByName(const string& name);
    
    // Gets the Secp256k1 curve parameters (this is the curve used by Bitcoin).
    DomainParameters GetSecp256k1Curve();
    
    // Gets the Secp112r1 curve parameters.
    DomainParameters GetSecp112r1Curve();
}

#endif
