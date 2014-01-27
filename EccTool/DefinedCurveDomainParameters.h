//
//  DefinedCurveDomainParameters.h
//  EccTool
//
//  Created by Josh Strom on 1/26/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#ifndef EccTool_DefinedCurveDomainParameters_h
#define EccTool_DefinedCurveDomainParameters_h

#include "EccDefs.h"
#include <exception>

namespace ecc
{
    const vector<string> GetSupportedCurves();
    const DomainParameters GetCurveByName(const string& name);
    DomainParameters GetSecp256k1Curve();
    DomainParameters GetSecp112r1Curve();

    // The following are a few sets of standard curve parameters.
    // Curve definition found here: http://www.secg.org/collateral/sec2_final.pdf
    
    const vector<string> GetSupportedCurves()
    {
        vector<string> curves;
        
        // Add all supported curves by name.
        curves.push_back("secp256k1");
        curves.push_back("secp112r1");
        
        return curves;
    }
    
    const DomainParameters GetCurveByName(const string& name)
    {
        if(name == "secp256k1")
            return GetSecp256k1Curve();
        if(name == "secp112r1")
            return GetSecp112r1Curve();
        
        throw invalid_argument("Unsupported curve: " + name);
    }
    
    DomainParameters GetSecp256k1Curve()
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
    
    DomainParameters GetSecp112r1Curve()
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
}

#endif
