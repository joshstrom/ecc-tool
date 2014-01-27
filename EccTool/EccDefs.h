//
//  EccDefs.h
//  EccTool
//
//  Created by Josh Strom on 1/25/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
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
