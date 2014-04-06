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
#ifndef EccTool_EccDefs_h
#define EccTool_EccDefs_h

#include <string>
#include <ostream>

using namespace std;

namespace ecc
{
    // Defines the parameters that define the elliptic curve equation: y^2 = x^3 + ax + b mod p.
    struct DomainParameters
    {
        // The name of this set of domain parameters.
        const string name;
        
        // The prime number which defines the field (Fp) in which the curve operates.
        const string p;
        
        // The two coefficients which define the curve:
        const string a;
        const string b;
        
        // The Generator or Base Point
        const string G;
        
        // The order of the curve generator point G.
        const string n;
        
        // The cofactor of the curve.
        const string h;
    };
}

#endif
